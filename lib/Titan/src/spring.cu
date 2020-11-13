//
// Created by Jacob Austin on 5/17/18.
//
#define GLM_FORCE_PURE
#include "spring.h"
#include <cmath>

const double EDGE_DAMPING = 20; // f_damp = delta_v_along_spring*edge_damping_constant;

Vec Spring::getForce() { // computes force on right object. left force is - right force.
  //    Vec temp = (_right -> pos) - (_left -> pos);
  //    return _k * (_rest - temp.norm()) * (temp / temp.norm());

    Vec temp = (_left -> pos) - (_right -> pos);
    Vec spring_force = _k * (temp.norm() - _rest) * (temp / temp.norm());

    //spring_force += dot( (_left->vel - _right->vel) , temp/temp.norm() )*EDGE_DAMPING* (temp/temp.norm());
    return spring_force;
}

int Spring::getLeft() {
    return _left -> index;
}

int Spring::getRight() {
    return _right -> index;
}

void Spring::setForce() { // computes force on right object. left force is - right force.
    Vec f = getForce();
    _right -> force += f;
    _left -> force += -f;
}

// Copy constructor
Spring::Spring(const Spring &other) {
    _k = other._k;
    _rest = other._rest;
    _diam = other._diam;
    _break_force = other._break_force;
    _curr_force = other._curr_force;
    _max_stress = other._max_stress;
    _broken = other._broken;
    _left = nullptr;
    _right = nullptr;
    _actuation = 0.0;
    _compute = true;
}

Spring::Spring(const CUDA_SPRING & spr) {
    this -> _k = spr._k;
    this -> _rest = spr._rest;
    this -> _diam = spr._diam;
    this -> _break_force = spr._break_force;
    this -> _curr_force = spr._curr_force;
    this -> _max_stress = spr._max_stress;
    this -> _broken = spr._broken;
    this -> _actuation = spr._actuation;
    this -> _compute = spr._compute;
}

void Spring::defaultLength() { _rest = (_left -> pos - _right -> pos).norm() ; } //sets Rest Lenght

void Spring::setLeft(Mass * left) {
    if (_left) {
        _left -> decrementRefCount();
    }

    _left = left;
    _left -> ref_count++;

} // sets left mass (attaches spring to mass 1)

void Spring::setRight(Mass * right) {
    if (_right) {
        _right -> decrementRefCount();
    }

    _right = right;
    _right -> ref_count++;
}

void Spring::operator=(CUDA_SPRING & spring) {

    _left = this->_left;
    _right = this->_right;

    _k = spring._k;
    _rest = spring._rest;
    _diam = spring._diam;
    _type = spring._type;
    _period = spring._period;
    _offset = spring._offset;
    _omega = spring._omega;
    _actuation = spring._actuation;
    _max_stress = spring._max_stress;
    _curr_force = spring._curr_force;
    _break_force = spring._break_force;
    _broken = spring._broken;
    _compute = spring._compute;

    arrayptr = this -> arrayptr;
}

CUDA_SPRING::CUDA_SPRING(const Spring & s) {
    _left = (s._left == nullptr) ? nullptr : s._left -> arrayptr;
    _right = (s._right == nullptr) ? nullptr : s. _right -> arrayptr;
    _k = s._k;
    _rest = s._rest;
    _diam = s._diam;
    _type = s._type;
    _period = s._period;
    _offset = s._offset;
    _omega = s._omega;
    _actuation = s._actuation;
    _max_stress = s._max_stress;
    _curr_force = s._curr_force;
    _break_force = s._break_force;
    _broken = s._broken;
    _compute = s._compute;
}

CUDA_SPRING::CUDA_SPRING(const Spring & s, CUDA_MASS * left, CUDA_MASS * right) {
    _left = left;
    _right = right;
    _k = s._k;
    _rest = s._rest;
    _diam = s._diam;
    _type = s._type;
    _period = s._period;
    _offset = s._offset;
    _omega = s._omega;
    _actuation = s._actuation;
    _max_stress = s._max_stress;
    _curr_force = s._curr_force;
    _break_force = s._break_force;
    _broken = s._broken;
    _compute = s._compute;
}
