/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NavigableWidget.h"

void UNavigableWidget::ReceiveFocus()
{
	this->IsFocused = true;
	this->HoverState();
}

void UNavigableWidget::LoseFocus()
{
	this->IsFocused = false;
	this->NormalState();
}
