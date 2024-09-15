#pragma once

/*
* cc/UiStackedWidget.hpp  Copyright (C) 2024  fairybow
*
* You should have received a copy of the GNU General Public License along with
* this program. If not, see <https://www.gnu.org/licenses/>.
*
* This file uses Qt 6. Qt is a free and open-source widget toolkit for creating
* graphical user interfaces. For more information, visit <https://www.qt.io/>.
*
* Updated: 2024-09-14
*/

#include "cc_namespaces.hpp"

#include <QHash>
#include <QStackedWidget>
#include <QVariant>

BEGIN_CC_UI_NAMESPACE

class StackedWidget : public QStackedWidget
{
	Q_OBJECT

public:
	using QStackedWidget::QStackedWidget;

	// Via Qt: "If the QStackedWidget is empty before this function is called, widget
	// becomes the current widget." Implies not otherwise!
	int addWidget(QWidget* widget)
	{
		auto index = QStackedWidget::addWidget(widget);
		emit widgetCountChanged(count());

		return index;
	}

	int insertWidget(int index, QWidget* widget)
	{
		auto insert_index = QStackedWidget::insertWidget(index, widget);
		emit widgetCountChanged(count());

		return insert_index;
	}

	void removeWidget(QWidget* widget)
	{
		m_data.remove(widget);
		QStackedWidget::removeWidget(widget);

		emit widgetCountChanged(count());
	}

	QVariant data(QWidget* widget) const
	{
		if (widget)
			return m_data[widget];

		return {};
	}

	template <typename DataT>
	DataT data(QWidget* widget) const
	{
		return data(widget).value<DataT>();
	}

	void setData(QWidget* widget, const QVariant& data)
	{
		if (widget)
			m_data[widget] = data;
	}

	QWidget* findData(const QVariant& data) const
	{
		for (auto it = m_data.begin(); it != m_data.end(); ++it)
		{
			if (it.value() != data) continue;
			return it.key();
		}

		return nullptr;
	}

	// In practice, for DDPW et al, we don't care about or use the stacked
	// widget's indexing (at least, I hope we don't care)
	// QWidget* widgetAt(int index) const
	// {
	// 	return widget(index);
	// }

signals:
	void widgetCountChanged(int count);

private:
	// Is this a problem?
	QHash<QWidget*, QVariant> m_data{};

}; // class Ui::StackedWidget

END_CC_UI_NAMESPACE
