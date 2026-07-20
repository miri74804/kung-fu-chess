#include "PieceGraphicsLibrary.h"
#include "../../Constants.h"
#include "../../io/PieceNotation.h"
#include <stdexcept>

namespace {
	// Plain arrays, size deduced from the initializer - no count to keep in
	// sync by hand. There are, and always will be, exactly 2 colors, 6 piece
	// types, and 5 animation states in this game.
	const Color ALL_COLORS[] = { Color::White, Color::Black };

	const PieceType ALL_PIECE_TYPES[] = {
		PieceType::KING, PieceType::QUEEN, PieceType::ROOK,
		PieceType::BISHOP, PieceType::KNIGHT, PieceType::PAWN
	};

	const PieceAnimationState ALL_STATES[] = {
		PieceAnimationState::Idle, PieceAnimationState::Move, PieceAnimationState::Jump,
		PieceAnimationState::LongRest, PieceAnimationState::ShortRest
	};

	// Every state's sprites/ folder holds exactly this many numbered frames.
	const int SPRITES_PER_STATE = 5;

	std::string pieceFolderName(Color color, PieceType type) {
		std::string name;
		name += PieceNotation::toSymbol(color);
		name += PieceNotation::toSymbol(type);
		return name;
	}

	std::string makeKey(Color color, PieceType type, PieceAnimationState state) {
		return pieceFolderName(color, type) + "/" + PieceAnimationStateNotation::toFolderName(state);
	}
}

void PieceGraphicsLibrary::loadAll(const std::string& assetsPiecesDir) {
	for (Color color : ALL_COLORS) {
		for (PieceType type : ALL_PIECE_TYPES) {
			for (PieceAnimationState state : ALL_STATES) {
				std::string stateDir = assetsPiecesDir + "/" + pieceFolderName(color, type)
					+ "/states/" + PieceAnimationStateNotation::toFolderName(state);

				PieceSprite sprite;
				sprite.config = AnimationConfig::loadFromFile(stateDir + "/config.json");

				for (int frame = 1; frame <= SPRITES_PER_STATE; ++frame) {
					Img img;
					img.read(stateDir + "/sprites/" + std::to_string(frame) + ".png",
						{ CELL_SIZE, CELL_SIZE }, /* keep_aspect */ true);
					sprite.frames.push_back(std::move(img));
				}

				sprites[makeKey(color, type, state)] = std::move(sprite);
			}
		}
	}
}

const PieceSprite& PieceGraphicsLibrary::get(Color color, PieceType type, PieceAnimationState state) const {
	auto it = sprites.find(makeKey(color, type, state));
	if (it == sprites.end()) {
		throw std::runtime_error("No sprite loaded for key: " + makeKey(color, type, state));
	}
	return it->second;
}
