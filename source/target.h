#pragma once

enum target_s : unsigned char {
	TargetSelf,
	TargetThrow, TargetThrowHitFighter, TargetAllThrow,
	TargetClose, TargetAllClose,
	TargetAlly, TargetAllAlly,
	TargetItems, TargetAllyItems, TargetAllAllyItems,
};
