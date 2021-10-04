#pragma once
#include "view.h"
#include "cursor.h"

enum StateType {
	Passthrough,
	Resize,
	Move
};

class MouseState {
public:
	StateType getType() { return type; }
	virtual ~MouseState() = default;

	virtual void update(const Cursor& cursor) = 0;

protected:
	StateType type;

};

class ResizeState : public MouseState {
public:

	ResizeState(View& view, const Cursor& cursor, uint32_t edges)
		: grabbedView(view), _edges(edges) {
		type = Resize;
		_originX = cursor.x() - view.x();
	}

	void update(const Cursor& cursor) {

	}


private:
	double _originX;
	double _originY;
	View& grabbedView;
	uint32_t _edges;
};

class MoveState : public MouseState {

public:
	MoveState(Ref<View> view, const Cursor& cursor) : _grabbedView(view) {
		type = Move;
		_originX = cursor.x() - view->x();
		_originY = cursor.y() - view->y();
	}

	void update(const Cursor& cursor) override {
		_grabbedView->move(cursor.x() - _originX, cursor.y() - _originY);
	}


private:
	double _originX;
	double _originY;
	Ref<View> _grabbedView;
};

class PassthroughState : public MouseState {
	public:
	PassthroughState() {
		type = Passthrough;
	}
	void update(const Cursor& cursor) override {}

};