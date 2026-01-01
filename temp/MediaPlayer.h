//#pragma once
//#include <QApplication>
//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
//
//
//
//class AudioPlayer : public QWidget
//{
//	Q_OBJECT
//private:
//	QMediaPlayer* player; // 播放器
//	bool loopPlay = true; // 是否循环播放
//	QString durationTime; // 总时长 mm:ss
//	QString positionTime; // 当前播放时间 mm:ss
//	QUrl getUrlFromItem(QListWidgetItem* item); // 获取 item 的用户数据
//	bool eventFilter(QObject* watched, QEvent* event); // 事件过滤处理
//	listWidget;
//public:
//	AudioPlayer(QWidget* parent = nullptr);
//private slots:
//	void do_stateChanged(QMediaPlayer::PlaybackState state); // 播放状态改变
//	void do_sourceChanged(const QUrl& media); // 播放源改变
//	void do_durationChanged(qint64 duration); // 总时长改变
//	void do_metaDataChanged(); // 元数据改变
//	void do_positionChanged(qint64 position); // 播放位置改变
//}