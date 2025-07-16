#include <pch.h>
#include "Collider.h"

Collider::Collider(Object* owner) : Component(owner), offset(0, 0), isTrigger(false)
{
}

