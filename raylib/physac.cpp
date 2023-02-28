// This file is part of SmallBASIC
//
// Plugin for raylib games library - https://www.raylib.com/
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"

#include <cstdio>

#include <raylib/raylib/src/raylib.h>
#define PHYSAC_IMPLEMENTATION
#include <physac/src/physac.h>

#include "include/var.h"
#include "include/module.h"
#include "include/param.h"
#include "physac.h"

static void v_setvec2(var_t *var, Vector2 &vec2) {
  map_init(var);
  v_setreal(map_add_var(var, "x", 0), vec2.x);
  v_setreal(map_add_var(var, "y", 0), vec2.y);
}

static void cmd_get_enabled(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setint(retval, body->enabled);
  }
}

static void cmd_get_position(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setvec2(retval, body->position);
  }
}

static void cmd_get_velocity(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setvec2(retval, body->velocity);
  }
}

static void cmd_get_force(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setvec2(retval, body->force);
  }
}

static void cmd_get_angular_velocity(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->angularVelocity);
  }
}

static void cmd_get_torque(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->torque);
  }
}

static void cmd_get_orient(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->orient);
  }
}

static void cmd_get_inertia(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->inertia);
  }
}

static void cmd_get_inverse_inertia(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->inverseInertia);
  }
}

static void cmd_get_mass(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->mass);
  }
}

static void cmd_get_inverse_mass(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->inverseMass);
  }
}

static void cmd_get_static_friction(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->staticFriction);
  }
}

static void cmd_get_dynamic_friction(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->dynamicFriction);
  }
}

static void cmd_get_restitution(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setreal(retval, body->restitution);
  }
}

static void cmd_get_use_gravity(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setint(retval, body->useGravity);
  }
}

static void cmd_get_is_grounded(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setint(retval, body->isGrounded);
  }
}

static void cmd_get_freeze_orient(var_s *self, var_s *retval) {
  PhysicsBody body = get_physics_body(self);
  if (body) {
    v_setint(retval, body->freezeOrient);
  }
}

void create(PhysicsBody body, var_p_t var, int id) {
  map_init_id(var, id);
  v_create_func(var, "enabled", cmd_get_enabled);
  v_create_func(var, "position", cmd_get_position);
  v_create_func(var, "velocity", cmd_get_velocity);
  v_create_func(var, "force", cmd_get_force);
  v_create_func(var, "angularVelocity", cmd_get_angular_velocity);
  v_create_func(var, "torque", cmd_get_torque);
  v_create_func(var, "orient", cmd_get_orient);
  v_create_func(var, "inertia", cmd_get_inertia);
  v_create_func(var, "inverseInertia", cmd_get_inverse_inertia);
  v_create_func(var, "mass", cmd_get_mass);
  v_create_func(var, "inverseMass", cmd_get_inverse_mass);
  v_create_func(var, "staticFriction", cmd_get_static_friction);
  v_create_func(var, "dynamicFriction", cmd_get_dynamic_friction);
  v_create_func(var, "restitution", cmd_get_restitution);
  v_create_func(var, "useGravity", cmd_get_use_gravity);
  v_create_func(var, "isGrounded", cmd_get_is_grounded);
  v_create_func(var, "freezeOrient", cmd_get_freeze_orient);
}

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

