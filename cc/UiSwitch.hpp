#pragma once

/*
* cc/UiSwitch.hpp  Copyright (C) 2024  fairybow
*
* You should have received a copy of the GNU General Public License along with
* this program. If not, see <https://www.gnu.org/licenses/>.
*
* This file uses Qt 6. Qt is a free and open-source widget toolkit for creating
* graphical user interfaces. For more information, visit <https://www.qt.io/>.
*
* Ui::Switch based on the hard work of user3559721, found here:
* <https://codereview.stackexchange.com/questions/249076/>
*
* Updated: 2024-09-17
*/

#include "cc_namespaces.hpp"

#include <QAbstractButton>
#include <QBrush>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QSize>
#include <QSizePolicy>

BEGIN_CC_UI_NAMESPACE

/// @todo Make track and ball stylable
/// @todo Dynamic sizing (shrinking to fit small spaces)
class Switch : public QAbstractButton
{
	Q_OBJECT;
	Q_PROPERTY(qreal position READ position WRITE setPosition)

public:
	Switch(QWidget* parent = nullptr)
		: QAbstractButton(parent)
	{
		setCheckable(true);
		setChecked(false);
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		m_animation->setTargetObject(this);
	}

	virtual ~Switch() = default;

	virtual QSize sizeHint() const
	{
		auto margins_space = _marginsSpace();

		return QSize
		(
			_trackWidth() + margins_space,
			_trackHeight() + margins_space
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
	virtual void enterEvent(QEnterEvent* event) override
	{
		setCursor(Qt::PointingHandCursor);
		QAbstractButton::enterEvent(event);
	}

	virtual void mouseReleaseEvent(QMouseEvent* event) override
	{
		QAbstractButton::mouseReleaseEvent(event);

		if (event->button() == Qt::LeftButton)
		{
			m_animation->setDuration(ANIMATION_DURATION);

			if (m_animation->state() != QPropertyAnimation::Running)
				m_animation->setPropertyName(BALL_POSITION_PROPERTY);

			m_animation->setStartValue(m_currentBallPosition);
			m_animation->setEndValue(_currentDestination());

			m_animation->start();
		}
	}

	virtual void paintEvent(QPaintEvent* event) override
	{
		(void)event;

		QPainter painter(this);
		painter.setRenderHints(QPainter::Antialiasing);
		painter.setPen(Qt::NoPen);

		_paintTrack(painter);
		_paintBall(painter);
	}

	virtual void resizeEvent(QResizeEvent* event) override
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

	qreal _trackHeight() const
	{
		return 2 * m_trackRadius;
	}

	// If the ball is smaller than the track, we can return 0. Otherwise, we
	// need a margin that accounts for the amount by which the ball is larger
	// than the track
	qreal _ballMargin() const
	{
		auto radius_diff = m_ballRadius - m_trackRadius;
		return (radius_diff < 0) ? 0 : radius_diff;
	}

	qreal _marginsSpace() const
	{
		return 2 * _ballMargin();
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

		auto margin = _ballMargin();
		auto margins_space = _marginsSpace();

		painter.drawRoundedRect
		(
			margin,
			margin,
			//width() - _marginsSpace(),
			_trackWidth(),
			//height() - _marginsSpace(),
			_trackHeight(),
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

END_CC_UI_NAMESPACE
