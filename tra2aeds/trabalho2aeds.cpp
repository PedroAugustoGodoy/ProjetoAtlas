
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

const int TAMANHO_ALFABETO = 26;

struct NoTrie {
  struct NoTrie* filhos[TAMANHO_ALFABETO];
  bool ehFimDaPalavra;
  unordered_map<string, vector<int>> indiceInvertido; // Armazena o índice invertido
};

struct NoTrie* obterNo() {
  struct NoTrie* pNode = new NoTrie;
  pNode->ehFimDaPalavra = false;

  for (int i = 0; i < TAMANHO_ALFABETO; i++)
    pNode->filhos[i] = NULL;

  return pNode;
}

void inserir(struct NoTrie* raiz, string chave, int numeroLinha) {
  struct NoTrie* pCrawl = raiz;

  for (int i = 0; i < chave.length(); i++) {
    int indice = chave[i] - 'a';
    if (!pCrawl->filhos[indice])
      pCrawl->filhos[indice] = obterNo();

    pCrawl = pCrawl->filhos[indice];
  }

  pCrawl->ehFimDaPalavra = true;
  pCrawl->indiceInvertido[chave].push_back(numeroLinha);
}

bool buscar(struct NoTrie* raiz, string chave) {
  struct NoTrie* pCrawl = raiz;

  for (int i = 0; i < chave.length(); i++) {
    int indice = chave[i] - 'a';
    if (!pCrawl->filhos[indice])
      return false;

    pCrawl = pCrawl->filhos[indice];
  }

  return (pCrawl != NULL && pCrawl->ehFimDaPalavra);
}

vector<int> buscarIndiceInvertido(struct NoTrie* raiz, string chave) {
  struct NoTrie* pCrawl = raiz;

  for (int i = 0; i < chave.length(); i++) {
    int indice = chave[i] - 'a';
    if (!pCrawl->filhos[indice])
      return {};

    pCrawl = pCrawl->filhos[indice];
  }

  if (pCrawl != NULL && pCrawl->ehFimDaPalavra)
    return pCrawl->indiceInvertido[chave];

  return {};
}

void destacarPalavraNaLinha(const string& linha, const string& palavra) {
  size_t pos = linha.find(palavra);
  if (pos == string::npos) {
    cout << linha << endl;
  } else {
    cout << linha.substr(0, pos) << "\033[1;31m" << palavra << "\033[0m"
         << linha.substr(pos + palavra.length()) << endl;
  }
}

vector<string> dividirEmPalavras(const string& frase) {
  vector<string> palavras;
  istringstream iss(frase);
  string palavra;
  while (iss >> palavra) {
    palavras.push_back(palavra);
  }
  return palavras;
}

bool correspondeFrase(const vector<string>& palavras, const vector<string>& palavrasLinha) {
  if (palavras.size() > palavrasLinha.size()) {
    return false; // A frase buscada é maior do que a linha
  }

  for (size_t i = 0; i <= palavrasLinha.size() - palavras.size(); i++) {
    bool encontrou = true;
    for (size_t j = 0; j < palavras.size(); j++) {
      if (palavras[j] != palavrasLinha[i + j]) {
        encontrou = false;
        break;
      }
    }
    if (encontrou) {
      return true; // A frase buscada foi encontrada na linha
    }
  }

  return false; // A frase buscada não foi encontrada na linha
}

int main() {
  string nomeArquivo = "robots.txt"; // Nome do arquivo de entrada

  // Abrindo o arquivo de entrada
  ifstream arquivoEntrada(nomeArquivo);
  if (!arquivoEntrada) {
    cout << "Falha ao abrir o arquivo de entrada." << endl;
    return 0;
  }

  struct NoTrie* raiz = obterNo();

  string linha;
  while (getline(arquivoEntrada, linha)) {
    ifstream arquivoTexto(linha); // Abre o arquivo de texto

    if (!arquivoTexto) {
      cout << "Falha ao abrir o arquivo: " << linha << endl;
      continue;
    }

    int numeroLinha = 1;
    while (getline(arquivoTexto, linha)) {
      istringstream iss(linha);
      string palavra;
      while (iss >> palavra) {
        inserir(raiz, palavra, numeroLinha);
      }
      numeroLinha++;
    }

    arquivoTexto.close(); // Fecha o arquivo de texto
  }

  arquivoEntrada.close(); // Fecha o arquivo de entrada

  // Interface de busca
  string consulta;
  cout << "Digite uma palavra ou frase para pesquisar (ou digite 'sair' para sair): ";
  while (getline(cin, consulta)) {
    if (consulta == "sair")
      break;

    if (consulta.find(' ') == string::npos) {
      vector<int> resultado = buscarIndiceInvertido(raiz, consulta);

      if (resultado.empty()) {
        cout << "Palavra não encontrada." << endl;
      } else {
        cout << "Palavra encontrada nas linhas: " << endl;
        for (int linha : resultado) {
          ifstream arquivoTexto(to_string(linha) + ".txt");
          if (!arquivoTexto) {
            cout << "Falha ao abrir o arquivo: " << to_string(linha) << ".txt" << endl;
            continue;
          }

          string linhaTexto;
          for (int i = 1; i <= linha; i++) {
            getline(arquivoTexto, linhaTexto);
          }
          arquivoTexto.close();

          cout << "Linha " << linha << ": ";
          destacarPalavraNaLinha(linhaTexto, consulta);
        }
      }
    } else {
      vector<string> palavras = dividirEmPalavras(consulta);

      ifstream arquivoEntrada("robots.txt"); // Nome do arquivo de entrada

      if (!arquivoEntrada) {
        cout << "Falha ao abrir o arquivo de entrada." << endl;
        return 0;
      }

      string linha;
      int numeroLinhaArquivo = 1;
      while (getline(arquivoEntrada, linha)) {
        ifstream arquivoTexto(linha); // Abre o arquivo de texto

        if (!arquivoTexto) {
          cout << "Falha ao abrir o arquivo: " << linha << endl;
          continue;
        }

       int numeroLinha = 1;
        while (getline(arquivoTexto, linha)) {
          vector<string> palavrasLinha = dividirEmPalavras(linha);

          if (correspondeFrase(palavras, palavrasLinha)) {
            cout << "Frase encontrada no arquivo " << linha << ", linha " << numeroLinha << ": " << linha << endl;
          }

          numeroLinha++;
        }

        arquivoTexto.close(); // Fecha o arquivo de texto
        numeroLinhaArquivo++;
      }

      arquivoEntrada.close(); // Fecha o arquivo de entrada
    }

    cout << endl << "Digite uma palavra ou frase para pesquisar (ou digite 'sair' para sair): ";
  }

  return 0;
}