#include "PieceGraphicsLibrary.h"
#include "../io/PieceNotation.h"
#include <array>
#include <stdexcept>

namespace {
	const std::array<Color, 2> ALL_COLORS = { Color::White, Color::Black };

	const std::array<PieceType, 6> ALL_PIECE_TYPES = {
		PieceType::KING, PieceType::QUEEN, PieceType::ROOK,
		PieceType::BISHOP, PieceType::KNIGHT, PieceType::PAWN
	};

	const std::array<PieceAnimationState, 5> ALL_STATES = {
		PieceAnimationState::Idle, PieceAnimationState::Move, PieceAnimationState::Jump,
		PieceAnimationState::LongRest, PieceAnimationState::ShortRest
	};

	std::string pieceFolderName(Color color, PieceType type) {
		std::string name;
		name += PieceNotation::toSymbol(color);
		name += PieceNotation::toSymbol(type);
		return name;
	}
}

std::string PieceGraphicsLibrary::makeKey(Color color, PieceType type, PieceAnimationState state) {
	return pieceFolderName(color, type) + "/" + PieceAnimationStateNotation::toFolderName(state);
}

void PieceGraphicsLibrary::loadAll(const std::string& assetsPiecesDir) {
	for (Color color : ALL_COLORS) {
		for (PieceType type : ALL_PIECE_TYPES) {
			for (PieceAnimationState state : ALL_STATES) {
				std::string stateDir = assetsPiecesDir + "/" + pieceFolderName(color, type)
					+ "/states/" + PieceAnimationStateNotation::toFolderName(state);

				PieceSprite sprite;
				sprite.config = AnimationConfig::loadFromFile(stateDir + "/config.json");

				for (int frame = 1; frame <= 5; ++frame) {
					Img img;
					img.read(stateDir + "/sprites/" + std::to_string(frame) + ".png");
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
