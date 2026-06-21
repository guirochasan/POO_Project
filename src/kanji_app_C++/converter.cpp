#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Função auxiliar para replicar o comportamento do ".join()" do Python
std::string join_strings(const std::vector<std::string>& vec, const std::string& delimiter) {
    std::string result = "";
    for (size_t i = 0; i < vec.size(); ++i) {
        result += vec[i];
        if (i < vec.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}

int main() {
    const std::string INPUT_FILE = "kanjis_original.json";
    const std::string OUTPUT_FILE = "kanjis.json";

    std::ifstream f_in(INPUT_FILE);
    if (!f_in.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << INPUT_FILE << "\n";
        return 1;
    }

    json data;
    try {
        f_in >> data;
    } catch (const json::parse_error& e) {
        std::cerr << "Erro ao analisar o JSON: " << e.what() << "\n";
        return 1;
    }
    f_in.close();

    json kanjis_convertidos = json::array();

    // Lambda (função anônima) para processar cada item, evitando repetição de código
    auto processar_item = [&kanjis_convertidos](const json& item) {
        std::string kanji = item.value("kanji", "");

        // Extraindo leituras ON e KUN e juntando em um único vetor (equivalente a readings_on + readings_kun)
        std::vector<std::string> leituras_combinadas;
        if (item.contains("readings_on") && item["readings_on"].is_array()) {
            for (const auto& r : item["readings_on"]) leituras_combinadas.push_back(r.get<std::string>());
        }
        if (item.contains("readings_kun") && item["readings_kun"].is_array()) {
            for (const auto& r : item["readings_kun"]) leituras_combinadas.push_back(r.get<std::string>());
        }

        // Extraindo os significados
        std::vector<std::string> meanings_vec;
        if (item.contains("meanings") && item["meanings"].is_array()) {
            for (const auto& m : item["meanings"]) meanings_vec.push_back(m.get<std::string>());
        }

        // Replicando o comportamento do ".join()" do Python
        std::string leitura = join_strings(leituras_combinadas, " / ");
        std::string significado = join_strings(meanings_vec, ", ");

        if (!kanji.empty()) {
            kanjis_convertidos.push_back({
                {"kanji", kanji},
                {"leitura", leitura},
                {"significado", significado}
            });
        }
    };

    // Verifica o tipo do JSON (isinstance(data, dict) ou isinstance(data, list))
    if (data.is_object()) {
        // Se for um dicionário, itera sobre os valores (equivalente a data.values())
        for (auto it = data.begin(); it != data.end(); ++it) {
            processar_item(it.value());
        }
    } else if (data.is_array()) {
        // Se for uma lista, itera diretamente
        for (const auto& item : data) {
            processar_item(item);
        }
    } else {
        std::cerr << "Formato de JSON desconhecido. Precisa ser Objeto (dict) ou Array (list).\n";
        return 1;
    }

    // Salvando no novo arquivo com indentação de 2 espaços
    std::ofstream f_out(OUTPUT_FILE);
    if (!f_out.is_open()) {
        std::cerr << "Erro ao salvar o arquivo " << OUTPUT_FILE << "\n";
        return 1;
    }
    
    // O parâmetro '2' garante o indent=2 do Python
    f_out << kanjis_convertidos.dump(2);
    f_out.close();

    std::cout << "Conversão concluída: " << kanjis_convertidos.size() 
              << " kanjis salvos em " << OUTPUT_FILE << "\n";

    return 0;
}