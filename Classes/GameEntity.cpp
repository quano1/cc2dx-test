// #include "GameEntity.h"
// // #include "HelloWorldScene.h"

// using namespace cocos2d;

// // int BaseEntity::s_id = 0;

// bool MovingEntity::initWithFile(std::string const &file)
// {
// 	if(!Sprite::initWithFile(file))
// 	{
// 		return false;
// 	}

// 	// seek_.connect(Simple::slot(lIns, &Exporter::on_init));

// 	scheduleUpdate();
// 	return true;
// }

// void MovingEntity::addForce(Vec2 const &force)
// {
// 	// if(force > max__force)

// 	// velocity_ += velocity;

// }

// // void MovingEntity::registerBehavior(Behavior const &behavior)
// // {
// // 	// steering_.connect(Simple::slot(behavior, &Behavior::on_message));
// // }

// void MovingEntity::update(float delta)
// {
// 	Sprite::update(delta);

// 	Vec2 steering_force = steering_.emit("seek", max_speed_, velocity_, getPosition(), target_);
// 	//Acceleration = Force/Mass
// 	Vec2 acceleration = steering_force / mass_;
// 	//update velocity
// 	velocity_ += acceleration * delta;
// 	//make sure vehicle does not exceed maximum velocity
// 	truncate(&velocity_, max_speed_);

// 	Vec2 new_pos = velocity_*delta + getPosition();
// 	this->setPosition(new_pos);

// 	if (velocity_.lengthSquared() > 0.00000001)
// 	{
// 		heading_ = (velocity_).getNormalized();
// 		side_ = heading_.getPerp();
// 	}

// 	float angle = CC_RADIANS_TO_DEGREES(atan2f(heading_.x, heading_.y));;
// 	this->setRotation(angle);
// }




