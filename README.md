# Human-Equipping-Incredible-SEnsor-Interface
HEISEI振り返りセンサー

# 使用方法
1. [https://look-back-heisei.herokuapp.com/](https://look-back-heisei.herokuapp.com/)にアクセスする
1. センサーの電源を入れる
1. センサーをベルトに取り付ける
1. 振り帰る
1. 平成を振り返る

# 開発環境
- マイコン: ESP32
- センサー: MPU9250
- ~~基板設計: KiCad 5.1.0~~
- 言語: Arduino
- 筐体設計: Fusion 360

# Requirements
## Arduinoライブラリ
- [bolderflight's MPU9250 library](https://github.com/bolderflight/MPU9250)
- [knolleary's pubsubclient library](https://github.com/knolleary/pubsubclient)
- [Adafruit's Adafruit_SSD1306 library](https://github.com/adafruit/Adafruit_SSD1306)

# TODO
- 保存/公開用回路図

# LICENSE
このプロジェクトは[MITライセンス](https://github.com/oita-kosen/Human-Equipping-Incredible-SEnsor-Interface/blob/master/LICENSE)のもと公開されています。以下の制限のもと利用可能です。
- このプロジェクト内のソースコードは誰でも自由に利用/改変/再頒布可能ですが，このスクリプトを利用するソースコード内の重要な箇所に著作権表示と[本ライセンス表示](https://github.com/oita-kosen/Human-Equipping-Incredible-SEnsor-Interface/blob/master/LICENSE)を付ける必要があります。  
- **このプロジェクト内のソースコードについて作者/著作権者は一切の責任を負いません**