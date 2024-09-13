#pragma once

/*
* cc/UiButton.hpp  Copyright (C) 2024  fairybow
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <https://www.gnu.org/licenses/>.
*
* Updated: 2024-09-12
*/

#include <QChar>
#include <QEvent>
#include <QEnterEvent>
#include <QFontDatabase>
#include <QString>
#include <QStyle>
#include <QToolButton>

#include <unordered_map>

class UiButton : public QToolButton
{
	Q_OBJECT

public:
	enum Icon
	{
		None = 0,
		Add,
		ChevronDown,
		ChevronLeft,
		ChevronRight,
		ChevronUp,
		Close,
		Ellipse,
		Menu,
		MenuOpen,
		Refresh
	};

	UiButton
	(
		const QString& text,
		QWidget* parent = nullptr,
		const QString& flaggedText = QString{}
	)
		:
		QToolButton(parent),
		m_label(text),
		m_flag(flaggedText)
	{
		_updateText();
	}

	// Lmao
	UiButton
	(
		Icon icon,
		QWidget* parent = nullptr,
		Icon flag = Icon{}
	)
		:
		UiButton
		(
			_iconText(icon),
			parent,
			_iconText(flag)
		)
	{
		setFont(_uiFont());
	}

	virtual ~UiButton() = default;

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

protected:
	virtual void enterEvent(QEnterEvent* event) override
	{
		QToolButton::enterEvent(event);
		m_hoveredOver = true;

		_updateText();
	}

	virtual void leaveEvent(QEvent* event) override
	{
		QToolButton::leaveEvent(event);
		m_hoveredOver = false;

		_updateText();
	}

private:
	static constexpr auto FLAG_PROPERTY = "flagged";
	static constexpr auto FONT_QRC = \
		":/ui-button/external/MaterialIcons-Regular.ttf";

	QString m_label;
	QString m_flag;
	bool m_flagged = false;
	bool m_hoveredOver = false;

	const std::unordered_map<Icon, QChar>& _iconHexMap() const
	{
		static const std::unordered_map<Icon, QChar> map =
		{
			{ Icon::Add, QChar(0xe145) },
			{ Icon::ChevronDown, QChar(0xe5cf) },
			{ Icon::ChevronLeft, QChar(0xe5cb) },
			{ Icon::ChevronRight, QChar(0xe5cc) },
			{ Icon::ChevronUp, QChar(0xe5ce) },
			{ Icon::Close, QChar(0xe5cd) },
			{ Icon::Ellipse, QChar(0xe061) },
			{ Icon::Menu, QChar(0xe5d2) },
			{ Icon::MenuOpen, QChar(0xe9bd) },
			{ Icon::Refresh, QChar(0xe5d5) }
		};

		return map;
	}

	const QString _iconText(Icon icon) const
	{
		auto& map = _iconHexMap();
		QChar font_icon{};

		auto it = map.find(icon);

		if (it != map.end())
			font_icon = it->second;

		return QString(font_icon);
	}

	const QFont _uiFont() const
	{
		static const auto id = QFontDatabase::addApplicationFont(FONT_QRC);
		QString font = QFontDatabase::applicationFontFamilies(id).at(0);

		return QFont(font);
	}

	void _updateText()
	{
		auto flagged = _flagShouldDisplay();

		flagged
			? setText(m_flag)
			: setText(m_label);

		_flagUpdateProperty(flagged);
	}

	bool _flagShouldDisplay() const
	{
		return m_flagged
			&& !m_hoveredOver
			&& !m_flag.isNull();
	}

	void _flagUpdateProperty(bool flagged)
	{
		setProperty(FLAG_PROPERTY, flagged);

		//style()->unpolish(this); Pretty sure this is wrong/bad/etc
		//style()->polish(this);
	}

}; // class UiButton
