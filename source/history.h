#pragma once

struct historyi {
	static constexpr unsigned history_max = 12;
	const char*		history[history_max];
	unsigned char	history_progress;
	unsigned		gethistorymax() const;
};
