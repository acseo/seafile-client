#include <QPainter>
#include <QApplication>
#include <QPixmap>
#include <QToolTip>

#include "QtAwesome.h"
#include "utils/utils.h"
#include "seafile-applet.h"
#include "rpc/rpc-client.h"
#include "starred-files-list-model.h"
#include "starred-file-item.h"

#include "starred-file-item-delegate.h"

namespace {

/**
            name
   icon 
            subtitle
 */

const int kMarginLeft = 5;
const int kMarginRight = 5;
const int kMarginTop = 5;
const int kMarginBottom = 5;
const int kPadding = 5;

const int kFileIconHeight = 36;
const int kFileIconWidth = 36;
const int kFileNameWidth = 175;
const int kFileNameHeight = 30;

const int kMarginBetweenFileIconAndName = 10;

const char *kFileNameColor = "#3F3F3F";
const char *kFileNameColorHighlighted = "#544D49";
const char *kSubtitleColor = "#959595";
const char *kSubtitleColorHighlighted = "#9D9B9A";
const int kFileNameFontSize = 12;
const int kSubtitleFontSize = 12;

const char *kFileItemBackgroundColor = "white";
const char *kFileItemBackgroundColorHighlighted = "#F9E0C7";


QString fitTextToWidth(const QString& text, const QFont& font, int width)
{
    static QString ELLIPSISES = "...";

	QFontMetrics qfm(font);
	QSize size = qfm.size(0, text);
	if (size.width() <= width)
		return text;				// it fits, so just display it

	// doesn't fit, so we need to truncate and add ellipses
	QSize sizeElippses = qfm.size(0, ELLIPSISES); // we need to cut short enough to add these
	QString s = text;
	while (s.length() > 20)     // never cut shorter than this...
	{
		int len = s.length();
		s = text.left(len-1);
		size = qfm.size(0, s);
		if (size.width() <= (width - sizeElippses.width()))
			break;              // we are finally short enough
	}

	return (s + ELLIPSISES);
}

QFont zoomFont(const QFont& font_in, double ratio)
{
    QFont font(font_in);

    if (font.pointSize() > 0) {
        font.setPointSize((int)(font.pointSize() * ratio));
    } else {
        font.setPixelSize((int)(font.pixelSize() * ratio));
    }

    return font;
}

QFont changeFontSize(const QFont& font_in, int size)
{
    QFont font(font_in);

    if (font.pointSize() > 0) {
        font.setPointSize(size);
    } else {
        font.setPixelSize(size);
    }

    return font;
}


} // namespace

StarredFileItemDelegate::StarredFileItemDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}

QSize StarredFileItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    StarredFileItem *item = getItem(index);
    return sizeHintForItem(option, item);
    if (!item) {
        return QStyledItemDelegate::sizeHint(option, index);
    }

}

QSize StarredFileItemDelegate::sizeHintForItem(const QStyleOptionViewItem &option,
                                               const StarredFileItem *item) const
{

    int width = kMarginLeft + kFileIconWidth
        + kMarginBetweenFileIconAndName + kFileNameWidth
        + kMarginRight + kPadding * 2;

    int height = kFileIconHeight + kPadding * 2 + kMarginTop + kMarginBottom;

    return QSize(width, height);
}

void StarredFileItemDelegate::paint(QPainter *painter,
                                    const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const
{
    StarredFileItem *item = getItem(index);

    paintItem(painter, option, item);
}

void StarredFileItemDelegate::paintItem(QPainter *painter,
                                        const QStyleOptionViewItem& option,
                                        const StarredFileItem *item) const
{
    QBrush backBrush;
    bool selected = false;
    const StarredFile& file = item->file();

    if (option.state & (QStyle::State_HasFocus | QStyle::State_Selected)) {
        backBrush = QColor(kFileItemBackgroundColorHighlighted);
        selected = true;

    } else {
        backBrush = QColor(kFileItemBackgroundColor);
    }

    painter->save();
    painter->fillRect(option.rect, backBrush);
    painter->restore();

    // paint file icon
    QPixmap icon = getIconForFile(file.name());
    QPoint file_icon_pos(kMarginLeft + kPadding, kMarginTop + kPadding);
    file_icon_pos += option.rect.topLeft();
    painter->save();
    painter->drawPixmap(file_icon_pos, icon);
    painter->restore();

    // Paint file name
    painter->save();
    QPoint file_name_pos = file_icon_pos + QPoint(kFileIconWidth + kMarginBetweenFileIconAndName, 0);
    QRect file_name_rect(file_name_pos, QSize(kFileNameWidth, kFileNameHeight));
    painter->setPen(QColor(selected ? kFileNameColorHighlighted : kFileNameColor));
    painter->setFont(changeFontSize(painter->font(), kFileNameFontSize));
    painter->drawText(file_name_rect,
                      Qt::AlignLeft | Qt::AlignTop,
                      fitTextToWidth(file.name(), option.font, kFileNameWidth),
                      &file_name_rect);

    // Paint subtitle
    QString subtitle, size, mtime;

    size = readableFileSize(file.size);
    mtime = translateCommitTime(file.mtime);

    subtitle = size + ", " + mtime;

    QPoint file_desc_pos = file_name_rect.bottomLeft() + QPoint(0, 5);
    QRect file_desc_rect(file_desc_pos, QSize(kFileNameWidth, kFileNameHeight));
    painter->setFont(changeFontSize(painter->font(), kSubtitleFontSize));
    painter->setPen(QColor(selected ? kSubtitleColorHighlighted : kSubtitleColor));

    painter->drawText(file_desc_rect,
                      Qt::AlignLeft | Qt::AlignTop,
                      fitTextToWidth(subtitle, option.font, kFileNameWidth),
                      &file_desc_rect);
    painter->restore();

}

QPixmap StarredFileItemDelegate::getIconForFile(const QString& name) const
{
    // TODO: return different icons basic on file type
    return QPixmap(":/images/repo.png");
}

StarredFileItem* StarredFileItemDelegate::getItem(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return NULL;
    }

    const StarredFilesListModel *model = (const StarredFilesListModel*)index.model();

    return (StarredFileItem *)(model->itemFromIndex(index));
}
