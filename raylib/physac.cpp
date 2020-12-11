// This file is part of SmallBASIC
//
// Plugin for raylib games library - https://www.raylib.com/
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"

#include <raylib/raylib/src/raylib.h>
#define PHYSAC_IMPLEMENTATION
#define PHYSAC_NO_THREADS
#include <raylib/src/physac.h>

#include "include/var.h"
#include "include/var_map.h"
#include "include/module.h"
#include "include/param.h"

static void v_setvec2(var_t *var, Vector2 &vec2) {
  map_init(var);
  v_setreal(map_add_var(var, "x", 0), vec2.x);
  v_setreal(map_add_var(var, "y", 0), vec2.y);
}

void create(PhysicsBody body, var_p_t var) {
  map_init(var);
  v_setint(map_add_var(var, "enabled", 0), body->enabled);
  v_setvec2(map_add_var(var, "position", 0), body->position);
  v_setvec2(map_add_var(var, "velocity", 0), body->velocity);
  v_setvec2(map_add_var(var, "force", 0), body->force);
  v_setreal(map_add_var(var, "angularVelocity", 0), body->angularVelocity);
  v_setreal(map_add_var(var, "torque", 0), body->torque);
  v_setreal(map_add_var(var, "orient", 0), body->orient);
  v_setreal(map_add_var(var, "inertia", 0), body->inertia);
  v_setreal(map_add_var(var, "inverseInertia", 0), body->inverseInertia);
  v_setreal(map_add_var(var, "mass", 0), body->mass);
  v_setreal(map_add_var(var, "inverseMass", 0), body->inverseMass);
  v_setreal(map_add_var(var, "staticFriction", 0), body->staticFriction);
  v_setreal(map_add_var(var, "dynamicFriction", 0), body->dynamicFriction);
  v_setreal(map_add_var(var, "restitution", 0), body->restitution);
  v_setint(map_add_var(var, "useGravity", 0), body->useGravity);
  v_setint(map_add_var(var, "isGrounded", 0), body->isGrounded);
  v_setint(map_add_var(var, "freezeOrient", 0), body->freezeOrient);
}

bool isUseGravity(PhysicsBody body) { return body->useGravity; }
bool isGrounded(PhysicsBody body) { return body->isGrounded; }
bool isFreezeOrient(PhysicsBody body) { return body->freezeOrient; }
Vector2 getPosition(PhysicsBody body) { return body->position; }
Vector2 getVelocity(PhysicsBody body) { return body->velocity; }
Vector2 getForce(PhysicsBody body) { return body->force; }
void setEnabled(PhysicsBody body, bool value) { body->enabled = value; }
void setPosition(PhysicsBody body, Vector2 position) { body->position = position; }
void setVelocity(PhysicsBody body, Vector2 velocity) { body->velocity = velocity;}
void setForce(PhysicsBody body, Vector2 force) { body->force = force;}
void setAngularVelocity(PhysicsBody body, float angularVelocity) { body->angularVelocity = angularVelocity;}
void setTorque(PhysicsBody body, float torque) { body->torque = torque;}
void setOrient(PhysicsBody body, float orient) { body->orient = orient;}
void setInertia(PhysicsBody body, float inertia) { body->inertia = inertia;}
void setInverseInertia(PhysicsBody body, float inverseInertia)  { body->inverseInertia = inverseInertia;}
void setMass(PhysicsBody body, float mass) { body->mass = mass;}
void setInverseMass(PhysicsBody body, float inverseMass) { body->inverseMass = inverseMass;}
void setStaticFriction(PhysicsBody body, float staticFriction) { body->staticFriction = staticFriction;}
void setDynamicFriction(PhysicsBody body, float dynamicFriction){ body->dynamicFriction = dynamicFriction;}
void setRestitution(PhysicsBody body, float restitution) { body->restitution = restitution;}
void setUseGravity(PhysicsBody body, bool useGravity) { body->useGravity = useGravity;}
void setIsGrounded(PhysicsBody body, bool isGrounded) { body->isGrounded = isGrounded;}
void setFreezeOrient(PhysicsBody body, bool freezeOrient) { body->freezeOrient = freezeOrient;}

