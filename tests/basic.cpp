#include "../ft_to_atlas.hh"


// TODO: do something for edge case where font letters demanding more size than give width and height
// since it is not of high priority, I'll get back to it some time later.

int main() {

	std::vector<std::string_view> fonts;

	// since visual studio demands absolute path, below path will vary.
#ifdef _WIN32
	fonts.emplace_back("Roboto-Regular.ttf");
	fonts.emplace_back("Roboto-Bold.ttf");
#endif
#ifdef __linux
	fonts.emplace_back("./Roboto-Regular.ttf");
	fonts.emplace_back("./Roboto-Bold.ttf");
#endif

	// size for each font, as per their respective index.
	std::vector<uint32_t> sizes = { 18,24 };

	uint8_t *a = create_font_atlas(256, 256, fonts, sizes, true, 0);

	if (!a) {
		// Error msg;
	}

	return 0;
}