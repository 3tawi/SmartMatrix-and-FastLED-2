/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 */

#ifndef PatternDigitalClock_H
#define PatternDigitalClock_H

class PatternDigitalClock : public Drawable {
private:
    // used for the random based animations
    int16_t dx;
    int16_t dy;
    int16_t dz;
    int16_t dsx;
    int16_t dsy;
    static const byte SNAKE_LENGTH = 32;
    CRGB colors[SNAKE_LENGTH];
    uint8_t initialHue;
    enum Direction {
        UP, DOWN, LEFT, RIGHT
    };//
    struct Pixel {
        uint8_t x;
        uint8_t y;
    };
    struct Snake {
        Pixel pixels[SNAKE_LENGTH];
        Direction direction;
        void newDirection() {
            switch (direction) {
                case UP:
                case DOWN:
                    direction = random(0, 2) == 1 ? RIGHT : LEFT;
                    break;
                case LEFT:
                case RIGHT:
                    direction = random(0, 2) == 1 ? DOWN : UP;
                default:
                    break;
            }
        }
        void shuffleDown() {
            for (byte i = SNAKE_LENGTH - 1; i > 0; i--) {
                pixels[i] = pixels[i - 1];
            }
        }
        void reset() {
            direction = UP;
            for (int i = 0; i < SNAKE_LENGTH; i++) {
                pixels[i].x = 0;
                pixels[i].y = 0;
            }
        }
        void move() {
            switch (direction) {
                case UP:
                    pixels[0].y = (pixels[0].y + 1) % kMatrixHeight;
                    break;
                case LEFT:
                    pixels[0].x = (pixels[0].x + 1) % kMatrixWidth;
                    break;
                case DOWN:
                    pixels[0].y = pixels[0].y == 0 ? kMatrixHeight - 1 : pixels[0].y - 1;
                    break;
                case RIGHT:
                    pixels[0].x = pixels[0].x == 0 ? kMatrixWidth - 1 : pixels[0].x - 1;
                    break;
            }
        }
        void draw(CRGB colors[SNAKE_LENGTH]) {
            for (byte i = 0; i < SNAKE_LENGTH; i++) {
              colors[i] %= (255 - i * (255 / SNAKE_LENGTH));
              effects.Pixel(pixels[i].x, pixels[i].y, colors[i]);
            }
        }
    };
    static const int snakeCount = 4;
    Snake snakes[snakeCount];

public:
    PatternDigitalClock() {
        name = (char *)"DigitalClock";
        for (int i = 0; i < snakeCount; i++) {
            Snake* snake = &snakes[i];
            snake->reset();
        }
    }

    void start() {
      dy = 3600;
    }    

    void drawsnake() {
     fill_palette(colors, SNAKE_LENGTH, initialHue++, 5, effects.MesgPalette, 255, LINEARBLEND);
     for (int i = 0; i < snakeCount; i++) {
      
      Snake* snake = &snakes[i];
      snake->shuffleDown();
      if (random(10) > 7) {
        snake->newDirection();
      }
      snake->move();
      snake->draw(colors);
    }
  }
    // show just one layer
    void ShowNoiseLayer(byte layer, byte colorrepeat, byte colorshift) {
      rgb24 *buffer = backgroundLayer.backBuffer();
      for (uint8_t i = 0; i < kMatrixWidth; i++) {
        uint32_t ioffset = noise_scale_x * (i - kMatrixWidth);
        for (uint8_t j = 0; j < kMatrixHeight; j++) {
          uint16_t ij = XY(i, j);
          CRGB color = CRGB(buffer[ij].red, buffer[ij].green, buffer[ij].blue);
          if (color) {
            uint32_t joffset = noise_scale_y * (j - kMatrixHeight);
            byte data = inoise16(noise_x + ioffset, noise_y + joffset, noise_z) >> 8;
            uint8_t olddata = noise;
            uint8_t newdata = scale8(olddata, noisesmoothing) + scale8(data, 256 - noisesmoothing);
            data = newdata;
            noise = data;
            color += effects.ColorFromMesgPalette(colorrepeat * (noise + colorshift), olddata);
            buffer[ij] = color;
          }
          
        }
      }
    }

    unsigned int drawFrame() { 
      noise_y += dy;
      noise_x += dx;
      noise_z += dz;
      effects.ClearFrame();
      drawHMTime(25);
      drawsnake();
      ShowNoiseLayer(0, 8, 2);
      drawampm(GPFont16x16, 47);
      drawMesg(textmsg, GPFont16x16, 16, 2);
      effects.DimAll(254);
      effects.ShowFrame();
      EVERY_N_SECONDS(10) {
        effects.CyclePalette(1);
      }       
        return 0;
    }
};

#endif
