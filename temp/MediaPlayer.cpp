//#include <QApplication>
//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
//#include <QFileDialog>
//#include "MediaPlayer.h"
//
//
//AudioPlayer::AudioPlayer(QWidget* parent)
//	: QWidget(parent)
//{
//	// 由于我们使用纯代码创建界面，因此不需要 ui 变量
//
//	player = new QMediaPlayer(this);
//
//	installEventFilter(this); // 安装事件过滤器
//	setAcceptDrops(true); // 允许拖放
//
//	QAudioOutput* audioOutput = new QAudioOutput(this);
//	player->setAudioOutput(audioOutput);
//
//	// 连接播放器信号到槽函数
//	connect(player, &QMediaPlayer::positionChanged, this, &AudioPlayer::do_positionChanged);
//	connect(player, &QMediaPlayer::playbackStateChanged, this, &AudioPlayer::do_stateChanged);
//	connect(player, &QMediaPlayer::sourceChanged, this, &AudioPlayer::do_sourceChanged);
//	connect(player, &QMediaPlayer::durationChanged, this, &AudioPlayer::do_durationChanged);
//	connect(player, &QMediaPlayer::metaDataChanged, this, &AudioPlayer::do_metaDataChanged);
//}
//
//QUrl AudioPlayer::getUrlFromItem(QListWidgetItem* item)
//{
//    QVariant itemData = item->data(Qt::UserRole);    //获取用户数据
//    QUrl source = itemData.value<QUrl>();    //QVariant转换为QUrl类型
//    return source;
//}
//
//bool AudioPlayer::eventFilter(QObject* watched, QEvent* event)
//{
//    if (event->type() != QEvent::KeyPress)      //不是KeyPress事件，退出
//        return QWidget::eventFilter(watched, event);
//
//    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
//    if (keyEvent->key() != Qt::Key_Delete)      //按下的不是Delete键，退出
//        return QWidget::eventFilter(watched, event);
//
//    if (watched == listWidget)
//    {
//        QListWidgetItem* item = listWidget->takeItem(listWidget->currentRow());
//        delete  item;
//    }
//    return true;    //表示事件已经被处理
//}