#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    // Nomes dos arquivos de entrada e saída
    const std::string INPUT_FILE = "kanji_data.json"; // O arquivo com o dataset que você baixou
    const std::string CLEAN_OUT_FILE = "kanji_no_romaji.json";
    const std::string JLPT_OUT_FILE = "jlpt_listas.json";

    std::ifstream f_in(INPUT_FILE);
    if (!f_in.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo " << INPUT_FILE << "!\n";
        std::cerr << "Certifique-se de que o arquivo existe na mesma pasta do executavel.\n";
        return 1;
    }

    json data;
    try {
        f_in >> data;
    } catch (const json::parse_error& e) {
        std::cerr << "Erro ao analisar o JSON de entrada: " << e.what() << "\n";
        return 1;
    }
    f_in.close();

    // Dicionário limpo (apenas com o essencial)
    json kanji_limpo = json::object();
    
    // Objeto para armazenar as listas por nível do JLPT (N5 a N2)
    json jlpt_listas = {
        {"N5", json::array()},
        {"N4", json::array()},
        {"N3", json::array()},
        {"N2", json::array()}
    };

    int contagem_limpos = 0;

    // Iterando pelo objeto JSON original (onde as chaves são os próprios kanjis)
    for (auto& [kanji_key, item] : data.items()) {
        int jlpt = -1; // -1 significa que não tem nível JLPT listado

        if (item.contains("jlpt") && !item["jlpt"].is_null()) {
            jlpt = item["jlpt"].get<int>();
        }

        // Cria o novo item limpo, ignorando heisig_en, name_readings, notes, etc.
        json clean_item = {
            {"kanji", kanji_key},
            {"jlpt", jlpt},
            {"meanings", item.value("meanings", json::array())},
            {"kun_readings", item.value("kun_readings", json::array())},
            {"on_readings", item.value("on_readings", json::array())}
        };

        kanji_limpo[kanji_key] = clean_item;
        contagem_limpos++;

        // Distribui para a lista de JLPT correspondente (ignorando N1)
        if (jlpt == 5) {
            jlpt_listas["N5"].push_back(kanji_key);
        } else if (jlpt == 4) {
            jlpt_listas["N4"].push_back(kanji_key);
        } else if (jlpt == 3) {
            jlpt_listas["N3"].push_back(kanji_key);
        } else if (jlpt == 2) {
            jlpt_listas["N2"].push_back(kanji_key);
        }
    }

    // Salvando o dicionário limpo
    std::ofstream f_clean(CLEAN_OUT_FILE);
    if (f_clean.is_open()) {
        f_clean << kanji_limpo.dump(2); // Indentação de 2 espaços
        f_clean.close();
        std::cout << "Sucesso: " << contagem_limpos << " kanjis salvos em " << CLEAN_OUT_FILE << "\n";
    }

    // Salvando as listas do JLPT
    std::ofstream f_jlpt(JLPT_OUT_FILE);
    if (f_jlpt.is_open()) {
        f_jlpt << jlpt_listas.dump(2);
        f_jlpt.close();
        std::cout << "Sucesso: Listas de N5 a N2 salvas em " << JLPT_OUT_FILE << "\n";
        
        // Imprime um resumo da quantidade por nível
        std::cout << " -> N5: " << jlpt_listas["N5"].size() << " kanjis\n";
        std::cout << " -> N4: " << jlpt_listas["N4"].size() << " kanjis\n";
        std::cout << " -> N3: " << jlpt_listas["N3"].size() << " kanjis\n";
        std::cout << " -> N2: " << jlpt_listas["N2"].size() << " kanjis\n";
    }

    return 0;
}