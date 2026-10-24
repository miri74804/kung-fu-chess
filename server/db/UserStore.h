#pragma once

#include <mutex>
#include <string>
#include <sqlite3.h>

// Thin RAII wrapper around the raw sqlite3 C API - just the handful of
// queries this project needs (create-or-verify an account, update a
// rating). Guarded by its own mutex, the same pattern as every other piece
// of shared state in this codebase (engineMutex, seatsMutex, ...), since
// it's called both from ixwebsocket callback threads (login) and the
// server's tick thread (rating updates at game end).
class UserStore {
public:
	// Opens (or creates) the SQLite file at dbPath and ensures the users
	// table exists - throws std::runtime_error if either step fails.
	explicit UserStore(const std::string& dbPath);
	~UserStore();

	UserStore(const UserStore&) = delete;
	UserStore& operator=(const UserStore&) = delete;

	struct LoginResult {
		bool success;
		std::string reason; // only meaningful when !success, e.g. "wrong_password"
		int rating;
	};

	// Looks up username; if it doesn't exist yet, creates the account
	// (hashed+salted password, rating = STARTING_ELO) and succeeds. If it
	// already exists, verifies the password and succeeds/fails accordingly.
	LoginResult login(const std::string& username, const std::string& password);

	void updateRating(const std::string& username, int newRating);

private:
	std::string hashPassword(const std::string& password, const std::string& salt) const;
	std::string generateSalt() const;

	sqlite3* db = nullptr;
	mutable std::mutex dbMutex;
};
