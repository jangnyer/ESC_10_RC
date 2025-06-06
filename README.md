# ESC_10_RC

# 🎯 RC 수신기 기반 RGB LED 제어 시스템 (Arduino + R9D)

본 프로젝트는 R9D 수신기의 9개 채널 중 3개(CH5, CH3, CH2)를 사용하여,  
다음의 3가지 기능을 구현한 시스템입니다.

1. ✅ LED 전원 ON/OFF 제어 (CH5)
2. ✅ LED 밝기 제어 (CH2)
3. ✅ RGB 3색 LED 색상 변화 제어 (CH3)

---

## 📹 시연 영상

아래 영상에서는 시스템의 **세 가지 동작이 모두 정상적으로 작동함**을 확인할 수 있으며,  
**R9D ↔ Arduino ↔ LED** 간의 배선 구조도 영상에서 함께 설명하였습니다.

👉 [YouTube 시연 영상 보러가기](https://youtu.be/XP7uYyeY1nc)

---

## 🧰 사용 부품 및 연결 구성

| 부품명         | 핀 번호 / 채널     | 설명                          |
|----------------|---------------------|-------------------------------|
| Arduino UNO    | -                   | 주 제어 보드                  |
| R9D 수신기     | CH5 (→ A2)          | 전원 스위치 신호              |
|                | CH3 (→ A0)          | 색상 조절 (스로틀 스틱)      |
|                | CH2 (→ A1)          | 밝기 조절 (엘리베이터 스틱)  |
| RGB LED        | R → D9              | 빨간색 출력                   |
|                | G → D10             | 초록색 출력                   |
|                | B → D11             | 파란색 출력                   |
| On/Off LED     | D6                  | 전원 상태 표시                |
| 밝기 LED       | D5                  | 밝기 수준 표시                |
| GND            | GND 연결            | 공통 접지                     |
| VCC (+5V)      | 수신기 및 LED용 전원 |                               |

> ※ RGB LED는 **공통 음극(Common Cathode)** 타입을 사용하였습니다.

---

## 📂 프로젝트 구조

```
/RGB-R9D-Control/
├── src/
│   └── main.cpp          # 메인 아두이노 코드
├── lib/
│   └── PinChangeInterrupt
├── platformio.ini        # PlatformIO 빌드 설정
└── README.md             # 설명서 (본 파일)
```

---

## 💡 주요 기능 요약

- **CH5 (A2)**  
  → PWM 폭을 계산해 1500us 기준으로 HIGH/LOW 구분  
  → `D6` 전원 LED를 ON/OFF 제어  

- **CH2 (A1)**  
  → PWM 입력을 0~255 범위로 맵핑하여 `D5` 밝기 LED에 출력  
  → `analogWrite()`로 밝기 제어

- **CH3 (A0)**  
  → PWM을 HSV 색상환 값으로 변환해 R/G/B 핀에 각각 출력  
  → 부드러운 색상 전환 구현

---

## 🛠️ 개발 환경

- **Arduino UNO**
- **PlatformIO + VSCode**
- **PinChangeInterrupt 라이브러리 사용**
- **시리얼 디버그용 출력 포함**

---

## 📎 기타

- 각 기능별 코드에는 상세한 주석이 포함되어 있어 기능별 구조를 쉽게 파악할 수 있습니다.
- R9D 수신기와 아두이노 사이의 PWM 신호는 `PinChangeInterrupt`를 사용하여 처리됩니다.
- 시리얼 모니터를 통해 입력되는 PWM 값을 실시간으로 확인할 수 있습니다.

---


