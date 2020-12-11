// This file is part of SmallBASIC
//
// Plugin for raylib games library - https://www.raylib.com/
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith


#pragma once

void create(PhysicsBody body, var_p_t map);
bool isUseGravity(PhysicsBody body);
bool isGrounded(PhysicsBody body);
bool isFreezeOrient(PhysicsBody body);
Vector2 getPosition(PhysicsBody body);
Vector2 getVelocity(PhysicsBody body);
Vector2 getForce(PhysicsBody body);
void setEnabled(PhysicsBody body, bool value);
void setPosition(PhysicsBody body, Vector2 value);
void setVelocity(PhysicsBody body, Vector2 value);
void setForce(PhysicsBody body, Vector2 value);
void setAngularVelocity(PhysicsBody body, float value);
void setTorque(PhysicsBody body, float value);
void setOrient(PhysicsBody body, float value);
void setInertia(PhysicsBody body, float value);
void setInverseInertia(PhysicsBody body, float value);
void setMass(PhysicsBody body, float value);
void setInverseMass(PhysicsBody body, float value);
void setStaticFriction(PhysicsBody body, float value);
void setDynamicFriction(PhysicsBody body, float value);
void setRestitution(PhysicsBody body, float value);
void setUseGravity(PhysicsBody body, bool value);
void setIsGrounded(PhysicsBody body, bool value);
void setFreezeOrient(PhysicsBody body, bool value);
