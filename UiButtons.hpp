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
* This file uses cc/cc.qrc, which uses Google Material Icons. Google Material
* Icons are free and open-source. For more information, visit
* <https://fonts.google.com/icons>.
*
* Updated: 2024-09-13
*/

#include <QChar>
#include <QEvent>
#include <QEnterEvent>
#include <QFontDatabase>
#include <QPushButton>
#include <QString>
#include <QStyle>
#include <QToolButton>

#include <unordered_map>

namespace Ui
{
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

	template <typename QButtonT>
	class ButtonBase : public QButtonT
	{
	public:
		ButtonBase
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
		ButtonBase
		(
			Icon icon,
			QWidget* parent = nullptr,
			Icon flag = Icon{}
		)
			:
			ButtonBase
			(
				_iconText(icon),
				parent,
				_iconText(flag)
			)
		{
			QButtonT::setFont(_uiFont());
		}

		virtual ~ButtonBase() = default;

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
			setFlagged(!flagged());
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
		static constexpr auto FLAG_PROPERTY = "flagged";
		static constexpr auto FONT_QRC = \
			":/cc/external/MaterialIcons-Regular.ttf";

		QString m_label;
		QString m_flag;
		bool m_flagged = false;
		bool m_hoveredOver = false;

		const std::unordered_map<Icon, QChar>& _iconHexMap() const
		{
			static const std::unordered_map<Icon, QChar> map =
			{
				{ Add, QChar(0xe145) },
				{ ChevronDown, QChar(0xe5cf) },
				{ ChevronLeft, QChar(0xe5cb) },
				{ ChevronRight, QChar(0xe5cc) },
				{ ChevronUp, QChar(0xe5ce) },
				{ Close, QChar(0xe5cd) },
				{ Ellipse, QChar(0xe061) },
				{ Menu, QChar(0xe5d2) },
				{ MenuOpen, QChar(0xe9bd) },
				{ Refresh, QChar(0xe5d5) }
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

	}; // class Ui::ButtonBase

	class Button : public ButtonBase<QPushButton>
	{
		Q_OBJECT

	public:
		using ButtonBase<QPushButton>::ButtonBase;
	};

	class ToolButton : public ButtonBase<QToolButton>
	{
		Q_OBJECT

	public:
		using ButtonBase<QToolButton>::ButtonBase;
	};

} // namespace Ui
