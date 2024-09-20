#pragma once

/*
* cc/UiDropDownPageWidget.hpp  Copyright (C) 2024  fairybow
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

#include <QComboBox>
#include <QHash>
#include <QPointer>
#include <QString>
#include <QUuid>

BEGIN_CC_UI_NAMESPACE

/// @todo Abstract away all possible
class DropDownPageWidget : public AbstractPageWidget
{
	Q_OBJECT

public:
	DropDownPageWidget(QWidget* parent = nullptr)
		: AbstractPageWidget(parent)
	{
		_setupComboBox();
	}

	~DropDownPageWidget() = default;

	int addPage(QWidget* widget, const QString& label, const QVariant& data = {}) override
	{
		// Add widget and set user data for the widget, if provided
		if (!addWidget(widget, data))
			return -1;

		// Associate widget with an ID for retrieval (duplicates are allowed, so
		// we can't use combo box text and also we would NEVER do that anyway
		// would we now)
		auto id = QUuid::createUuid();
		m_comboBoxItemIdsToWidgets[id] = widget;

		// Add label to combo box along with widget association ID
		comboBox()->addItem(label, id);

		// Return the combo box index of the new item
		return comboBox()->findData(id);
	}

	QComboBox* comboBox() const
	{
		return  qobject_cast<QComboBox*>(controller());
	}

	int indexOf(const QWidget* widget) const override
	{
		auto it = m_comboBoxItemIdsToWidgets.begin();
		auto end = m_comboBoxItemIdsToWidgets.end();

		while (it != end)
		{
			if (it.value() == widget)
				return comboBox()->findData(it.key());

			++it; // I'm so sorry little guy; I'll never forget you again
		}

		return -1;
	}

	QWidget* widgetAt(int index) const override
	{
		auto id = comboBox()->itemData(index).value<QUuid>();
		return m_comboBoxItemIdsToWidgets[id];
	}

	int currentIndex() const override
	{
		return comboBox()->currentIndex();
	}

public slots:
	void setCurrentIndex(int index) override
	{
		comboBox()->setCurrentIndex(index);
	}

private:
	/// @todo Need to be really clear about these temp data hashes used to find
	/// our widgets. They get confusing.
	QHash<QUuid, QPointer<QWidget>> m_comboBoxItemIdsToWidgets{};

	void _setupComboBox()
	{
		auto combo_box = new QComboBox;
		combo_box->setEditable(false);
		combo_box->setDuplicatesEnabled(true);

		combo_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

		connect
		(
			combo_box,
			&QComboBox::currentIndexChanged,
			this,
			&DropDownPageWidget::onControllerIndexChanged
		);

		setController(combo_box);
	}

}; // class Ui::DropDownPageWidget

END_CC_UI_NAMESPACE
