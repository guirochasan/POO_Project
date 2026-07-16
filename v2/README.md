# Sistema de Repetição Espaçada de Kanji (Kanji SRS)

**Aluno:** Guilherme Rocha Sampaio  
**Matrícula:** 20250019133 
**Disciplina:** Programação Orientada a Objetos (UFPB 2026.1)  

---

## 1. Descrição do Domínio
O sistema modela uma aplicação de **Repetição Espaçada (SRS)** dedicada ao aprendizado da língua japonesa. Nele, itens de estudo (Kanjis e Vocabulários) são agendados dinamicamente para revisão. À medida que o estudante acerta ou erra as respostas, as classes ajustam os tempos de intervalo e geram prioridades para exibição em sessões ativas de estudo.

---

## 2. Diagrama de Classes UML (TP1 & TP2)

```mermaid
classDiagram
    class revisable {
        <<interface>>
        +review(bool success)* void
    }

    class study_item {
        <<abstract>>
        -id_ string
        -word_ string
        -interval_days_ int
        -ease_factor_ float
        +calculate_priority()* float
        +display() void
        +id() string
        +word() string
        +interval_days() int
        +ease_factor() float
    }

    class kanji_card {
        -character_ string
        -stroke_count_ int
        +calculate_priority() float
        +display() void
        +review(bool success) void
    }

    class vocabulary_card {
        <<final>>
        -part_of_speech_ string
        +calculate_priority() float
        +display() void
    }

    class session_stats {
        -total_reviewed_ int
        -total_correct_ int
        +record_attempt(bool success) void
        +calculate_accuracy() float
    }

    class study_session {
        -stats_ unique_ptr~session_stats~
        -items_ vector~study_item*~
        +add_study_item(study_item* item) void
        +run_simulation() void
        +show_results() void
    }

    revisable <|.. kanji_card : implementa (Heranca Multipla)
    study_item <|-- kanji_card : herda (Subclasse)
    study_item <|-- vocabulary_card : herda (Subclasse Final)
    study_session *-- session_stats : composicao (Ciclo de vida atrelado)
    study_session o-- study_item : agregacao (Referencias independentes)
