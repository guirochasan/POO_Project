# POO_Project

Guilherme Rocha Sampaio
Matricula: 20250019133

## Smart flash card

Esse é um aplicativo que segue o modelo padrão de flashcards inteligentes como anki, porém com a adição de um algoritmo de pop up inteligente que calcula horários aleatórios e faz um pop up aparecer na tela do computador para chamar o usuário a engajar no processo de memorização.

## Diagrama UML de Classes

```mermaid
classDiagram
    class Kanji {
        -string literal_
        +Kanji(string literal)
        +~Kanji()
        +get_literal() string
    }

    class Deck {
        -string name_
        -vector~unique_ptr~Kanji~~ kanjis_
        +Deck(string name)
        +~Deck()
        +add_kanji(string literal) void
        +get_name() string
        +listar_kanjis() void
    }

    class User {
        -string name_
        +User(string name)
        +~User()
        +get_name() string
        +estudar() void
    }

    class StudySession {
        -shared_ptr~Deck~ deck_ref_
        -shared_ptr~User~ user_ref_
        +StudySession(shared_ptr~Deck~ deck, shared_ptr~User~ user)
        +~StudySession()
        +rodar_sessao() void
    }

    %% Relacionamentos baseados no ciclo de vida do código
    Deck "1" *-- "0..*" Kanji : Composição (Q3-A / Q4-A via unique_ptr)
    StudySession "0..*" o-- "1" Deck : Agregação (Q3-B / Q4-A via shared_ptr)
    StudySession "0..*" o-- "1" User : Agregação (Q3-B / Q4-A via shared_ptr)

## Descrição das Classes
Kanji: Representa a unidade básica de aprendizado. Armazena o próprio ideograma (literal_) e informações relevantes do caractere, como a quantidade de traços (stroke_count_).

Deck: Funciona como um baralho de cartas. É o contêiner responsável por agrupar e organizar múltiplos objetos Kanji sob um tema ou nível específico.

User: Representa o estudante que interage com o sistema. Mantém o estado de progressão individual, como o seu nome e nível atual de proficiência (level_).

StudySession: Representa um evento de estudo localizado no tempo. Associa um usuário a um baralho específico por uma duração determinada (duration_minutes_), permitindo registrar o progresso e o número de itens revisados.

## Relacionamentos e Ciclo de Vida
A arquitetura do sistema gerencia o ciclo de vida dos objetos de forma estrita:

Composição (Deck contém Kanji): O baralho atua como o "dono" absoluto dos ideogramas. Um Kanji instanciado não possui existência lógica isolada fora do baralho ao qual foi adicionado. Portanto, se um objeto Deck for destruído, todos os Kanji atrelados a ele são automaticamente eliminados da memória em conjunto.

Agregação (StudySession referencia Deck e User): A sessão de estudo precisa conectar um aluno a um material para acontecer, mas ela não atua como "dona" de nenhum deles. Quando a StudySession é finalizada e tem seu objeto destruído, tanto o Deck quanto o User permanecem intactos para serem utilizados em sessões futuras.
