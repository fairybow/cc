#pragma once

/*
* cc/UiButtons.hpp  Copyright (C) 2024  fairybow
*
* You should have received a copy of the GNU General Public License along with
* this program. If not, see <https://www.gnu.org/licenses/>.
*
* This file uses Qt 6. Qt is a free and open-source widget toolkit for creating
* graphical user interfaces. For more information, visit <https://www.qt.io/>.
*
* This file uses cc/cc.qrc, which uses Google Material Symbols. Google Material
* Symbols are free and open-source. For more information, visit
* <https://fonts.google.com/icons>.
*
* Ui::Switch based on the hard work of user3559721, found here:
* <https://codereview.stackexchange.com/questions/249076/>
*
* Updated: 2024-09-14
*/

#include "cc_namespaces.hpp"

#include <QBrush>
#include <QChar>
#include <QEvent>
#include <QEnterEvent>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QMargins>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSize>
#include <QString>
#include <QToolButton>

#include <array>
#include <utility>

BEGIN_CC_UI_NAMESPACE

/// @todo Update for Symbols
enum Icon
{
	None = 0,
	Add,
	ChevronLeft,
	ChevronRight,
	Close,
	Ellipse,
	ExpandLess,
	ExpandMore,
	FormatPaint,
	Menu,
	MenuOpen,
	Refresh
};

inline constexpr static std::array<std::pair<Icon, QChar>, 11> ICON_HEX_MAP =
{
	{
		{ Add, QChar(0xe145) },
		{ ChevronLeft, QChar(0xe5cb) },
		{ ChevronRight, QChar(0xe5cc) },
		{ Close, QChar(0xe5cd) },
		{ Ellipse, QChar(0xe061) }, // not Material
		{ ExpandLess, QChar(0xe5ce) },
		{ ExpandMore, QChar(0xe5cf) },
		{ FormatPaint, QChar(0xe243) },
		{ Menu, QChar(0xe5d2) },
		{ MenuOpen, QChar(0xe9bd) },
		{ Refresh, QChar(0xe5d5) }
	}
};

inline const QFont uiFont()
{
	constexpr static auto qrc = \
		":/cc/external/MaterialSymbolsRounded-VariableFont_FILL,GRAD,opsz,wght.ttf";

	static const auto id = QFontDatabase::addApplicationFont(qrc);
	QString font = QFontDatabase::applicationFontFamilies(id).at(0);

	return font;
}

inline constexpr QChar getIconHex(Icon icon)
{
	for (const auto& pair : ICON_HEX_MAP)
	{
		if (pair.first == icon)
			return pair.second;
	}

	return {};
}

/// @todo Make track and ball stylable
class Switch : public QAbstractButton
{
	Q_OBJECT	Q_PROPERTY(qreal position READ position WRITE setPosition)

public:
	Switch(QWidget* parent)
		: QAbstractButton(parent)
	{
		setCheckable(true);
		setChecked(false);
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		m_animation->setTargetObject(this);
	}

	virtual ~Switch() = default;

	QSize sizeHint() const
	{
		auto margins_space = _marginsSpace();

		return QSize
		(
			_trackWidth() + margins_space,
			(2 * m_trackRadius) + margins_space
		);
	}

	qreal position() const
	{
		return m_currentBallPosition;
	}

	void setPosition(qreal position)
	{
		m_currentBallPosition = position;
		update();
	}

	// QAbstractButton::setChecked does not emit the clicked signal
	void setChecked(bool checked)
	{
		QAbstractButton::setChecked(checked);
		m_currentBallPosition = _currentDestination();
	}

protected:
	void enterEvent(QEnterEvent* event) override
	{
		setCursor(Qt::PointingHandCursor);
		QAbstractButton::enterEvent(event);
	}

	void mouseReleaseEvent(QMouseEvent* event) override
	{
		QAbstractButton::mouseReleaseEvent(event);

		if (event->button() == Qt::LeftButton)
		{
			m_animation->setDuration(ANIMATION_DURATION);

			if (m_animation->state() != QAbstractAnimation::Running)
				m_animation->setPropertyName(BALL_POSITION_PROPERTY);

			m_animation->setStartValue(m_currentBallPosition);
			m_animation->setEndValue(_currentDestination());

			m_animation->start();
		}
	}

	void paintEvent(QPaintEvent* /*event*/) override
	{
		QPainter painter(this);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
		painter.setPen(Qt::NoPen);

		_paintTrack(painter);
		_paintBall(painter);
	}

	void resizeEvent(QResizeEvent* event) override
	{
		QAbstractButton::resizeEvent(event);
		m_currentBallPosition = _currentDestination();
	}

private:
	QPropertyAnimation* m_animation = new QPropertyAnimation(this);
	constexpr static auto ANIMATION_DURATION = 100;
	constexpr static auto BALL_POSITION_PROPERTY = "position";

	qreal m_trackRadius = 10;
	qreal m_ballRadius = 8;
	qreal m_currentBallPosition = _offPosition();

	qreal _trackWidth() const
	{
		return 3.6 * m_trackRadius;
	}

	// If the ball is smaller than the track, we can return 0. Otherwise, we
	// need a margin that accounts for the amount by which the ball is larger
	// than the track
	qreal _trackMargin() const
	{
		auto radius_diff = m_ballRadius - m_trackRadius;
		return (radius_diff < 0) ? 0 : radius_diff;
	}

	qreal _marginsSpace() const
	{
		return 2 * _trackMargin();
	}

	qreal _offPosition() const
	{
		return m_ballRadius > m_trackRadius
			? m_ballRadius
			: m_trackRadius;
	}

	qreal _onPosition() const
	{
		return _trackWidth() + _marginsSpace() - _offPosition();
	}

	qreal _currentDestination() const
	{
		return isChecked() ? _onPosition() : _offPosition();
	}

	QBrush _ballBrush() const
	{
		if (isEnabled())
		{
			return isChecked()
				? palette().highlight()
				: palette().light();
		}

		return palette().mid();
	}

	QBrush _trackBrush() const
	{
		if (isEnabled())
		{
			return isChecked()
				? palette().highlight()
				: palette().dark();
		}

		return palette().shadow();
	}

	qreal _opacity(qreal opacity) const
	{
		return isEnabled() ? opacity : opacity * 0.3;
	}

	void _paintTrack(QPainter& painter) const
	{
		painter.setBrush(_trackBrush());
		painter.setOpacity(_opacity(0.5));

		auto margin = _trackMargin();
		auto margins_space = _marginsSpace();

		painter.drawRoundedRect
		(
			margin,
			margin,
			width() - _marginsSpace(),
			height() - _marginsSpace(),
			m_trackRadius,
			m_trackRadius
		);
	}

	void _paintBall(QPainter& painter) const
	{
		painter.setBrush(_ballBrush()); // lol
		painter.setOpacity(_opacity(1.0));

		painter.drawEllipse
		(
			m_currentBallPosition - m_ballRadius,
			_offPosition() - m_ballRadius,
			2 * m_ballRadius,
			2 * m_ballRadius
		);
	}

}; // class Ui::Switch

/// @todo Rework
template <typename QButtonT>
class FlagButtonBase : public QButtonT
{
public:
	FlagButtonBase
	(
		const QString& text,
		QWidget* parent = nullptr,
		const QString& flaggedText = QString{}
	)
		:
		QButtonT(parent),
		m_label(text),
		m_flag(flaggedText)
	{
		_updateText();
	}

	// Lmao
	FlagButtonBase
	(
		Icon icon,
		QWidget* parent = nullptr,
		Icon flag = Icon{}
	)
		:
		FlagButtonBase
		(
			_iconText(icon),
			parent,
			_iconText(flag)
		)
	{
		QButtonT::setFont(uiFont());
	}

	virtual ~FlagButtonBase() = default;

	bool hoveredOver() const noexcept
	{
		return m_hoveredOver;
	}

	QString label() const
	{
		return m_label;
	}

	void setLabel(const QString& text)
	{
		m_label = text;
	}

	void setLabel(Icon icon)
	{
		m_label = _iconText(icon);
	}

	QString flag() const
	{
		return m_flag;
	}

	void setFlag(const QString& text)
	{
		m_flag = text;
	}

	void setFlag(Icon icon)
	{
		m_flag = _iconText(icon);
	}

	bool flagged() const
	{
		return m_flagged;
	}

	void setFlagged(bool flagged = true)
	{
		m_flagged = flagged;

		_updateText();
	}

	void toggleFlagged()
	{
		setFlagged(!m_flagged);
	}

protected:
	virtual void enterEvent(QEnterEvent* event) override
	{
		QButtonT::enterEvent(event);
		m_hoveredOver = true;

		_updateText();
	}

	virtual void leaveEvent(QEvent* event) override
	{
		QButtonT::leaveEvent(event);
		m_hoveredOver = false;

		_updateText();
	}

private:
	constexpr static auto FLAG_PROPERTY = "flagged";

	QString m_label;
	QString m_flag;
	bool m_flagged = false;
	bool m_hoveredOver = false;

	const QString _iconText(Icon icon) const
	{
		return getIconHex(icon);
	}

	void _updateText()
	{
		auto flagged = _flagShouldDisplay();

		flagged
			? QButtonT::setText(m_flag)
			: QButtonT::setText(m_label);

		// This is for style sheets
		QButtonT::setProperty(FLAG_PROPERTY, flagged);

		QButtonT::update();
	}

	bool _flagShouldDisplay() const
	{
		return m_flagged
			&& !m_hoveredOver
			&& !m_flag.isNull();
	}

}; // class Ui::FlagButtonBase

class FlagButton : public FlagButtonBase<QPushButton>
{
	Q_OBJECT

public:
	using FlagButtonBase<QPushButton>::FlagButtonBase;
};

class FlagToolButton : public FlagButtonBase<QToolButton>
{
	Q_OBJECT

public:
	using FlagButtonBase<QToolButton>::FlagButtonBase;
};

END_CC_UI_NAMESPACE
