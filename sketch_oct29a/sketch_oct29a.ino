
const int buttonPin = 2;
const int RledPin = 3;
const int GledPin = 5;
const int BledPin = 6;

struct RGB {
  int r;
  int g;
  int b;
};

RGB colors[] = {
  {255, 0, 0},
  {255, 255, 0},
  {0, 255, 0},
  {0, 255, 255},
  {0, 0, 255}
};

const int colorCount = 5;
int colorIndex = 0;

unsigned long colorStart = 0;

const unsigned long blinkStart = 2000;
const unsigned long fadeStart = 3000;
const unsigned long fadeDuration = 1000;

const unsigned long blinkInterval = 250;

const unsigned long breathingDuration = 5000;

int buttonState = HIGH;
int lastButtonReading = HIGH;

unsigned long debounceTime = 0;
const unsigned long debounceDelay = 30;

bool waitingForSecondClick = false;
unsigned long firstClickTime = 0;

const unsigned long doubleClickTime = 350;

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);

  colorStart = millis();

  showColor(colors[colorIndex]);
}

void loop() {

  unsigned long now = millis();

  checkButton(now);

  unsigned long elapsed = now - colorStart;

  RGB currentColor = colors[colorIndex];

  RGB nextColor = colors[
    min(colorIndex + 1, colorCount - 1)
  ];

  if (colorIndex == colorCount - 1) {

    float progress =
      (float)(elapsed % breathingDuration)
      / breathingDuration;

    float brightness;

    if (progress < 0.5) {
      brightness = progress * 2;
    }
    else {
      brightness = (1.0 - progress) * 2;
    }

    RGB breathingColor;

    breathingColor.r = currentColor.r * brightness;
    breathingColor.g = currentColor.g * brightness;
    breathingColor.b = currentColor.b * brightness;

    showColor(breathingColor);

  }

  else {
    if (elapsed < blinkStart) {

      showColor(currentColor);

    }

    else if (elapsed < fadeStart) {

      if ((elapsed / blinkInterval) % 2 == 0) {
        showColor(currentColor);
      }
      else {
        showColor({0, 0, 0});
      }

    }

    else if (elapsed < fadeStart + fadeDuration) {

      float progress =
        (float)(elapsed - fadeStart)
        / fadeDuration;

      RGB fadeColor;

      fadeColor.r =
        currentColor.r +
        (nextColor.r - currentColor.r) * progress;

      fadeColor.g =
        currentColor.g +
        (nextColor.g - currentColor.g) * progress;

      fadeColor.b =
        currentColor.b +
        (nextColor.b - currentColor.b) * progress;

      showColor(fadeColor);

    }

    else {

      colorIndex++;

      colorStart += fadeStart + fadeDuration;

    }

  }

}

void showColor(RGB color) {

  analogWrite(RledPin, 255 - color.r);
  analogWrite(GledPin, 255 - color.g);
  analogWrite(BledPin, 255 - color.b);

}

void checkButton(unsigned long now) {

  int reading = digitalRead(buttonPin);

  // Debounce
  if (reading != lastButtonReading) {
    debounceTime = now;
  }

  if (now - debounceTime > debounceDelay) {

    if (reading != buttonState) {

      buttonState = reading;

      if (buttonState == HIGH) {

        if (waitingForSecondClick &&
            now - firstClickTime <= doubleClickTime) {

          if (colorIndex > 0) {
            colorIndex--;
          }

          colorStart = now;

          waitingForSecondClick = false;

        }

        else {

          waitingForSecondClick = true;
          firstClickTime = now;

        }

      }

    }

  }

  lastButtonReading = reading;

  if (waitingForSecondClick &&
      now - firstClickTime > doubleClickTime) {

    waitingForSecondClick = false;

  }

}
