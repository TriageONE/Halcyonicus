//
// Created by Triage on 2/5/2023.
//
#include "entity.h"
#include <utility>

LOCATION ENTITY::getLocation() {
    return this->location;
}

int ENTITY::getMaxHealth() const {
    return this->maxHealth;
}

int ENTITY::getCurrentHealth() const {
    return this->currentHealth;
}

bool ENTITY::isInvincible() const {
    return this->invincible;
}

nlohmann::json ENTITY::getData() {
    return this->jsonData;
}

void ENTITY::setLocation(LOCATION l) {
    this->location = l;
}

void ENTITY::setMaxHealth(int newMaxHealth) {
    this->maxHealth = newMaxHealth;
}

void ENTITY::setCurrentHealth(int newCurrentHealth) {
    this->currentHealth = newCurrentHealth;
}

void ENTITY::setInvincibility(bool inv) {
    this->invincible = inv;
}

void ENTITY::setData(nlohmann::json json) {
    this->jsonData = std::move(json);
}

void ENTITY::decrementHealth() {
    if(this->currentHealth > 0)
        this->currentHealth--;
}

void ENTITY::incrementHealth() {
    if(this->currentHealth < this->maxHealth)
        this->currentHealth++;
}

void ENTITY::changeHealthBy(int hp) {
    this->currentHealth = std::clamp(this->currentHealth + hp, 0, this->maxHealth);
}

//TODO: GETER DONE.. SOON
std::string ENTITY::compileToPCAS() {
    std::stringstream pcas;

    /**
     * Format
     */
    return std::string();
}




