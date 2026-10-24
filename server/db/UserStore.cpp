#include "UserStore.h"
#include "../../core/Constants.h"
#include <picosha2.h>
#include <random>
#include <stdexcept>

UserStore::UserStore(const std::string& dbPath) {
	if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
		std::string message = db != nullptr ? sqlite3_errmsg(db) : "unknown error";
		throw std::runtime_error("Failed to open user database: " + message);
	}

	const char* createTable =
		"CREATE TABLE IF NOT EXISTS users ("
		"username TEXT PRIMARY KEY,"
		"salt TEXT NOT NULL,"
		"passwordHash TEXT NOT NULL,"
		"rating INTEGER NOT NULL"
		");";
	char* errMsg = nullptr;
	if (sqlite3_exec(db, createTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::string message = errMsg != nullptr ? errMsg : "unknown error";
		sqlite3_free(errMsg);
		throw std::runtime_error("Failed to create users table: " + message);
	}
}

UserStore::~UserStore() {
	if (db != nullptr) {
		sqlite3_close(db);
	}
}

std::string UserStore::generateSalt() const {
	static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	static std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> dist(0, static_cast<int>(sizeof(alphabet)) - 2);

	std::string salt;
	for (int i = 0; i < 16; ++i) {
		salt += alphabet[dist(rng)];
	}
	return salt;
}

std::string UserStore::hashPassword(const std::string& password, const std::string& salt) const {
	return picosha2::hash256_hex_string(salt + password);
}

UserStore::LoginResult UserStore::login(const std::string& username, const std::string& password) {
	std::lock_guard<std::mutex> lock(dbMutex);

	sqlite3_stmt* stmt = nullptr;
	sqlite3_prepare_v2(db, "SELECT salt, passwordHash, rating FROM users WHERE username = ?;", -1, &stmt, nullptr);
	sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
		std::string salt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		std::string storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		int rating = sqlite3_column_int(stmt, 2);
		sqlite3_finalize(stmt);

		if (hashPassword(password, salt) != storedHash) {
			return { false, "wrong_password", 0 };
		}
		return { true, "", rating };
	}
	sqlite3_finalize(stmt);

	// No such user yet - a first-time login creates the account.
	std::string salt = generateSalt();
	std::string hash = hashPassword(password, salt);

	sqlite3_stmt* insertStmt = nullptr;
	sqlite3_prepare_v2(db, "INSERT INTO users (username, salt, passwordHash, rating) VALUES (?, ?, ?, ?);", -1, &insertStmt, nullptr);
	sqlite3_bind_text(insertStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(insertStmt, 2, salt.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(insertStmt, 3, hash.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(insertStmt, 4, STARTING_ELO);
	sqlite3_step(insertStmt);
	sqlite3_finalize(insertStmt);

	return { true, "", STARTING_ELO };
}

void UserStore::updateRating(const std::string& username, int newRating) {
	std::lock_guard<std::mutex> lock(dbMutex);

	sqlite3_stmt* stmt = nullptr;
	sqlite3_prepare_v2(db, "UPDATE users SET rating = ? WHERE username = ?;", -1, &stmt, nullptr);
	sqlite3_bind_int(stmt, 1, newRating);
	sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}
