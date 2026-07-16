#pragma once

// Interface pura que define comportamento de um item revisável
class Reviewable {
public:
    virtual ~Reviewable() = default; // Destrutor virtual obrigatório
    
    // Método virtual puro para registrar o resultado do card (acerto/erro)
    virtual void review(bool success) = 0; 
};