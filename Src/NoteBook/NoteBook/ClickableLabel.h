#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMap>
#include <QPixmap>

typedef QMap<int, QPixmap>	IconMap;


class CClickableLabel : public QLabel
{
	Q_OBJECT

public:
	CClickableLabel(QWidget *parent);
	~CClickableLabel();

	void LoadIconForIndex(int index, const QString& iconPath);
	void SetIcon(int index);

protected:
	virtual void mousePressEvent(QMouseEvent *event);

signals:
	void LabelClicked();
	void LabelRightClicked();

private:
	IconMap			m_iconMap;
};

#endif // CLICKABLELABEL_H
