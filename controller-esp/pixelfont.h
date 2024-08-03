#ifndef PIXELFONT_H
#define PIXELFONT_H


class PixelFont {

public:
    virtual ~PixelFont() { }

    bool has_char(char c) {
        return get_jumps()[(int) c] != -1;
    }

    uint8_t get_width(char c) {
        return get_widths()[(int) c];
    };

    uint8_t get_octet(char c, uint8_t index) {
        size_t jump_index = get_jumps()[(int) c];

        if (index >= get_width(c)) {
            return 0x00;
        }

        return get_chars()[jump_index + index];
    }

    static uint8_t get_undercut(PixelFont& font1, char c1, PixelFont& font2, char c2) {
        if (c1 == ' ' || c2 == ' ') {
          return 0;
        }
        if (!font1.has_char(c1) || !(font2.has_char(c2))) {
            return 0;
        }

        uint8_t c1_width = font1.get_width(c1);
        uint8_t octet1 = font1.get_octet(c1, c1_width - 1);
        uint8_t octet2 = font2.get_octet(c2, 0);


        if (octet1 & octet2) {
            return 0;
        }
        if (octet1 & (octet2 >> 1)) {
            return 0;
        }
        if (octet1 & (octet2 << 1)) {
            return 0;
        }

        return 1;
    }

private:
    virtual const uint8_t* get_chars() = 0;
    virtual const int* get_jumps() = 0;
    virtual const int* get_widths() = 0;

};

#endif //PIXELFONT_H
