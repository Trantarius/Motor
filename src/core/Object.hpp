#pragma once
#include <memory>


struct Object{
	const std::shared_ptr<Object*> self_ptr = std::make_shared<Object*>(this);
	virtual ~Object(){
		*self_ptr = nullptr;
	}
};
