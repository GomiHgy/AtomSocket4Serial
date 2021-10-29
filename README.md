# AtomSocket4Serial
[AtomSocket](https://shop.m5stack.com/products/atom-socket-kit-hlw8023-jp-us)をシリアルでコントロールするサンプルアプリです。

<img src="https://user-images.githubusercontent.com/10735253/139429925-c884768a-9324-4584-9349-a3b376c2170f.png" height="240px">

## はじめに
リモートワーク時に遠隔の装置の電源をON/OFFしたかったので、AtomSocket用の簡易アプリケーションを作りました。

[M5Atom](https://shop.m5stack.com/collections/atom-series/products/atom-lite-esp32-development-kit)はESP32ベースで作られているためWifiやBluetoothを使えるとより良いのですが、
会社で無線を使うことが難しい為、使用許可の出ているシリアル通信を使うようにしています。

また、社内でシリアル通信用のクライアントとして[TeraTerm](https://ttssh2.osdn.jp/)を使用している為、TeraTermのシリアルポートの設定を弄らずに使える設定にしています。

## 使い方

1. AtomSocketの使い方を参考に電源を取り付け、M5AtomとPCをUSBケーブルで接続する。
2. TeraTermを開き、M5Atomの接続されているシリアルポートを選択する。

<img src="https://user-images.githubusercontent.com/10735253/139430303-d232938a-3e9d-4b72-a6f0-85cf3d3ff430.png" height="240px">

3. 各種コマンド+Enterを入力して制御する。(TeraTermの他の設定はデフォルトのままだが、念の為下記に記入)

##### 端末
<img src="https://user-images.githubusercontent.com/10735253/139430557-7a4e5a39-2135-485f-9310-fa3a214a3f3d.png" height="240px">

##### シリアルポート
<img src="https://user-images.githubusercontent.com/10735253/139430526-f4fe4147-b290-4e5d-b94a-0f5743a9d07c.png" height="320px">

## コマンド一覧
| 名称 | 機能 | 対応バージョン | 補足 |
| ------------- | ------------- | ------------- | ------------- |
| ON | 電源をONする | 1.0 |  |
| OFF | 電源をOFFする | 1.0 |  |
| VER | バージョンを表示する | 1.0 |  |

## 今後の予定
- [ ] 電力情報の取得機能
- [ ] ON時間の設定機能
