#include <Arduino.h>
#include "PinChangeInterrupt.h"  // 핀 변경 인터럽트 라이브러리

// === 인터럽트 핸들러 함수 선언 ===
// 각 채널에서 신호가 들어올 때 실행되는 함수들입니다.
void isrPowerInput();       // 전원 ON/OFF용 인터럽트
void isrColorInput();       // 색상 제어 인터럽트
void isrBrightnessInput();  // 밝기 제어 인터럽트

// === 주요 동작 함수 선언 ===
void handlePowerToggle();   // 전원 상태 변경
void adjustColor();         // 색상 변경
void adjustBrightness();    // 밝기 변경
void debugPrint();          // 시리얼 디버깅 출력

// === 핀 매핑 ===
// 각 수신기 채널에 해당하는 아날로그 핀에 연결합니다.
const int powerPin     = A2;  // CH5 (보통 스위치 타입 채널) → 전원 토글
const int colorPin     = A0;  // CH3 → 색상 조절 (스로틀 스틱)
const int brightPin    = A1;  // CH2 → 밝기 조절 (엘리베이터 스틱)

// === PWM 신호 측정용 변수 ===
// PWM 신호의 폭(마이크로초)을 저장하고, 새로운 신호가 들어왔는지 여부를 표시합니다.
volatile int           pulsePower     = 1500;  // 기본값: 1500us (중간값)
volatile unsigned long tPowerStart    = 0;
volatile bool          flagPower      = false;

volatile int           pulseColor     = 1500;
volatile unsigned long tColorStart    = 0;
volatile bool          flagColor      = false;

volatile int           pulseBrightness= 1500;
volatile unsigned long tBrightStart   = 0;
volatile bool          flagBright     = false;

// === 실시간으로 매핑된 PWM 값을 저장하는 변수 ===
int pwmValPower;
int pwmValColor;
int pwmValBright;

// === 출력 핀 설정 (LED) ===
const int redPin    = 9;   // R: 빨간색
const int greenPin  = 10;  // G: 초록색
const int bluePin   = 11;  // B: 파란색

const int ledPower  = 6;   // 시스템 ON/OFF 표시용 LED
const int ledBright = 5;   // 밝기 수준 표시용 LED

// RGB 값 저장 변수
int rVal, gVal, bVal;

void setup() {
  // === 입력 핀 설정 및 인터럽트 등록 ===
  pinMode(powerPin, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(powerPin), isrPowerInput, CHANGE);

  pinMode(colorPin, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(colorPin), isrColorInput, CHANGE);

  pinMode(brightPin, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(brightPin), isrBrightnessInput, CHANGE);

  // === 출력 핀 설정 ===
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(ledPower, OUTPUT);
  pinMode(ledBright, OUTPUT);

  Serial.begin(9600);  // 시리얼 통신 시작
}

void loop() {
  // 각각의 인터럽트가 발생하면 처리 후 플래그를 false로 초기화

  if (flagPower) {
    pwmValPower = pulsePower;
    handlePowerToggle();
    flagPower = false;
  }

  if (flagColor) {
    pwmValColor = pulseColor;
    adjustColor();
    flagColor = false;
  }

  if (flagBright) {
    pwmValBright = pulseBrightness;
    adjustBrightness();
    flagBright = false;
  }

  // 시리얼 출력 (디버깅용)
  debugPrint();
}

// === 인터럽트 함수 구현 ===

// CH5 (스위치) 입력 변화 감지
void isrPowerInput() {
  if (digitalRead(powerPin)) {
    tPowerStart = micros();  // 상승 에지에서 시간 기록
  } else {
    pulsePower = micros() - tPowerStart;  // 하강 에지에서 폭 계산
    flagPower = true;  // 새 신호 수신 표시
  }
}

// CH3 (스로틀) 입력 변화 감지
void isrColorInput() {
  if (digitalRead(colorPin)) {
    tColorStart = micros();
  } else if (tColorStart && !flagColor) {
    pulseColor = micros() - tColorStart;
    flagColor = true;
  }
}

// CH2 (엘리베이터) 입력 변화 감지
void isrBrightnessInput() {
  if (digitalRead(brightPin)) {
    tBrightStart = micros();
  } else if (tBrightStart && !flagBright) {
    pulseBrightness = micros() - tBrightStart;
    flagBright = true;
  }
}

// === 기능 함수 구현 ===

// 스위치 값에 따라 전원 표시 LED ON/OFF
void handlePowerToggle() {
  digitalWrite(ledPower, pwmValPower >= 1500 ? HIGH : LOW);
}

// CH2로 밝기를 0~255 범위로 조절
void adjustBrightness() {
  int level = map(pwmValBright, 1000, 2000, 0, 255);
  level = constrain(level, 0, 255);  // 범위 제한
  analogWrite(ledBright, level);     // 밝기 LED 출력
}

// HSV 색상환 기반으로 색상 변화
void adjustColor() {
  int hue = map(pwmValColor, 1000, 2000, 0, 255);
  hue = constrain(hue, 0, 255);

  // HSV → RGB 변환 알고리즘
  if (hue < 85) {
    rVal = 255 - hue * 3;
    gVal = hue * 3;
    bVal = 0;
  } else if (hue < 170) {
    int h2 = hue - 85;
    rVal = 0;
    gVal = 255 - h2 * 3;
    bVal = h2 * 3;
  } else {
    int h2 = hue - 170;
    rVal = h2 * 3;
    gVal = 0;
    bVal = 255 - h2 * 3;
  }

  // 각각의 색상 핀에 출력
  analogWrite(redPin, rVal);
  analogWrite(greenPin, gVal);
  analogWrite(bluePin, bVal);
}

// 디버깅용 시리얼 출력 함수
void debugPrint() {
  if (flagPower) {
    Serial.print("[POWER] PWM: "); Serial.println(pwmValPower);
  }
  if (flagColor) {
    Serial.print("[COLOR] PWM: "); Serial.println(pwmValColor);
  }
  if (flagBright) {
    Serial.print("[BRIGHTNESS] PWM: "); Serial.println(pwmValBright);
  }
}
