#pragma once

/*
* cc/UiTabPageWidget.hpp  Copyright (C) 2024  fairybow
*
* You should have received a copy of the GNU General Public License along with
* this program. If not, see <https://www.gnu.org/licenses/>.
*
* This file uses Qt 6. Qt is a free and open-source widget toolkit for creating
* graphical user interfaces. For more information, visit <https://www.qt.io/>.
*
* Updated: 2024-09-20
*/

#include "cc_namespaces.hpp"
#include "UiAbstractPageWidget.h"

#include <QHash>
#include <QPointer>
#include <QString>
#include <QTabBar>
#include <QUuid>

BEGIN_CC_UI_NAMESPACE

class TabPageWidget : public AbstractPageWidget
{
	Q_OBJECT

public:
	TabPageWidget(QWidget* parent = nullptr)
		: AbstractPageWidget(parent)
	{
		_setupTabBar();
	}

	~TabPageWidget() = default;

	int addPage(QWidget* widget, const QString& label, const QVariant& data = {}) override
	{
		// Add widget and set user data for the widget, if provided
		if (!addWidget(widget, data))
			return -1;

		// Associate widget with an ID for retrieval
		auto id = QUuid::createUuid();
		m_tabBarItemIdsToWidgets[id] = widget;

		auto tab_bar = tabBar();

		// Add tab along with widget association ID
		auto index = tab_bar->addTab(label);
		tab_bar->setTabData(index, id);

		// Return the tab bar index of the new item
		return index;
	}

	QTabBar* tabBar() const
	{
		return  qobject_cast<QTabBar*>(controller());
	}

	int indexOf(const QWidget* widget) const override
	{
		auto it = m_tabBarItemIdsToWidgets.constBegin();
		auto end = m_tabBarItemIdsToWidgets.constEnd();

		while (it != end)
		{
			if (it.value() == widget)
			{
				auto tab_bar = tabBar();

				for (auto i = 0; i < tab_bar->count(); ++i)
					if (tab_bar->tabData(i) == it.key())
						return i;
			}

			++it;
		}

		return -1;
	}

	QWidget* widgetAt(int index) const override
	{
		auto id = tabBar()->tabData(index).value<QUuid>();
		return m_tabBarItemIdsToWidgets[id];
	}

	int currentIndex() const override
	{
		return tabBar()->currentIndex();
	}

public slots:
	void setCurrentIndex(int index) override
	{
		tabBar()->setCurrentIndex(index);
	}

private:
	/// @todo Need to be really clear about these temp data hashes used to find
	/// our widgets. They get confusing.
	QHash<QUuid, QPointer<QWidget>> m_tabBarItemIdsToWidgets{};

	void _setupTabBar()
	{
		auto tab_bar = new QTabBar;
		tab_bar->setMovable(true);

		tab_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

		connect
		(
			tab_bar,
			&QTabBar::currentChanged,
			this,
			&TabPageWidget::onControllerIndexChanged
		);

		setController(tab_bar);
	}

}; // class Ui::TabPageWidget

END_CC_UI_NAMESPACE
