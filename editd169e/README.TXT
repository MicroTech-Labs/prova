★★★★★★★★★★★★★★★
　nuko.dll ver 1.00
　取扱説明書
★★★★★★★★★★★★★★★

■配布ファイル

nuko.dll　　プラグイン本体
nuko.ini　　プラグインプロファイル
README.TXT　これ

■プラグインについて

これはDiskExplorer用プラグインです。
このプラグインを使うと MEGA-SCSI用HDDイメージ(SFORM形式)
ならびに 似非RAM/SCCのSRAMイメージファイルを操作できます。

■動作環境

DiskExplorer本体に準じます。
DiskExplorer本体ファイルは下記サイトより入手できます。
http://hp.vector.co.jp/authors/VA013937/editdisk/index.html

プラグインメッセージは日本語で書いていますので、日本語環境以外では文字化けしま
す。

■プラグインの配置

nuko.dll と nuko.ini をDiskExplorer 本体と同じフォルダに配置してください。

■MEGA-SCSIのハードディスクイメージ操作方法について

DiskExplorerのディスクイメージを開くでイメージファイルを指定後、プロファイル選
択画面で 『MEGA-SCSI HDD (SFORM形式)』を選択してください。
次にパーティション選択画面が表示されますので、目的のパーティションを選択してく
ださい。
以後の操作は、通常のDiskExplorerと同等ですので本体に同梱のヘルプファイルを参照
してください。

SFORMのフォーマット形式は二種類(似非ASPIと旧来のもの)ありますが、どちらにも自
動判別で対応します。

パーティションの選択をやりなおす際も、DiskExplorer本体との仕様により、面倒です
がディスクイメージを開くからやり直す必要があります。

MEGA-SCSIのハードディスクイメージのうち、当プラグインで操作できるのは
SFORM(2)で作成されたものです。FAT16でフォーマットされたものは
『VMWARE plain disk』を選択して操作してください。

■似非RAM/SCCのディスクイメージ操作方法

DiskExplorerのディスクイメージを開くでイメージファイルを指定後、プロファイル選
択画面で 『ESE-RAM/SCC DISK』を選択してください。
以後の操作は、通常のDiskExplorerと同等ですので本体に同梱のヘルプファイルを参照
してください。

■著作権について

当プラグインはフリーソフトです。
誰でも自由に複製を作って使用することができます。

当プラグインは DiskExplorer Device I/O Plug-in Specification を元に作成されて
います。DiskExplorer Device I/O Plug-in Specification 部分の著作権はjunnno氏が
保有します。その他の部分は white cat が保有します。

■免責

このプラグインを利用した結果の障害について、white cat ならびに junnno氏は一切
の責任を負いません。

■連絡先

white cat <marin.psx@gmail.com>
