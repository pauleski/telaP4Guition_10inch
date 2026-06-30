# Guition JC8012P4A1 — Display ESPHome

## Descrição do Projeto
Este projeto configura o painel inteligente **Guition JC8012P4A1** (composto por um ESP32-P4 principal e um coprocessador ESP32-C6 para Wi-Fi/Bluetooth) rodando ESPHome com suporte a LVGL e touchscreen capacitivo. Foi desenvolvida uma interface rica com tema *Dark Glassmorphism*, contemplando uma tela inicial (screensaver) animada e uma tela de seleção de cafés completa.

---

## Estrutura do Workspace e Arquivos Necessários

Certifique-se de que os seguintes arquivos e pastas estão presentes no diretório raiz:

1. `display.yaml` — Arquivo principal de configuração do ESPHome.
2. `secrets.yaml` — Arquivo que contém as credenciais iniciais da rede Wi-Fi da sua residência/trabalho.
3. `wifi_manager.h` — Classe C++ auxiliar para a tela interativa de controle de credenciais Wi-Fi.
4. `fontes/` — Diretório com as fontes customizadas:
   - `PlaywriteDEGrund-Regular.ttf` (Fonte principal da interface)
   - `Inconsolata_Expanded-SemiBold.ttf` (Fonte estilo digital usada para o relógio da tela home)
5. `assets/` — Diretório contendo todas as imagens da interface gráfica (ícones, logos, planos de fundo).
6. `components/gsl3680/` — Pasta local com os arquivos do driver de touchscreen corrigidos para compatibilidade.

---

## Passos para Obter Êxito na Compilação

Para compilar este projeto em versões modernas do ESPHome (2026.4.x ou superior), os seguintes ajustes estruturais foram necessários e já estão aplicados na pasta do projeto:

### 1. Requisitos do Access Point (AP) no `display.yaml`
A senha do ponto de acesso de fallback (`ap -> password`) configurada no ESPHome deve ter **no mínimo 8 caracteres** (padrão de validação WPA). Caso contrário, a compilação falhará antes de iniciar. A conexão de Wi-Fi também aproveita o armazenamento persistente NVS (`enable_nvs_wifi: true`) para autoconexão a redes conhecidas.

### 2. Formatação dos Glyphs de Fontes no `display.yaml`
Nas versões mais recentes do ESPHome, o campo `glyphs` sob a configuração de `font` não aceita mais um array de inteiros (valores ordinais dos caracteres ASCII). Ele deve ser formatado como uma **string contendo todos os caracteres necessários**, escapando aspas e contra-barras corretamente:
```yaml
glyphs: " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ª°·..."
```

### 3. Ajuste do Driver Touchscreen `gsl3680`
A biblioteca externa obtida no repositório da comunidade utilizava chamadas obsoletas que causavam erro de compilação em C++. Para resolver isso, ela foi baixada localmente em `components/gsl3680/` e modificada:
- **Arquivo modificado**: `components/gsl3680/gsl3680.cpp`
- **Correção**: A chamada `this->mark_failed("I2C init error");` foi alterada para `this->mark_failed();`, pois a versão moderna do ESPHome não aceita strings de diagnóstico diretamente como argumento dessa função.
- **Configuração no YAML**: Foi adicionada a diretiva para utilizar o componente local:
  ```yaml
  external_components:
    - source:
        type: local
        path: components
      components: [gsl3680]
  ```

---

## Como Flashar (Comandos Necessários)

Siga os comandos abaixo no terminal do sistema:

### 1. Atualizar ou Instalar o ESPHome
Garante que a versão instalada no seu Python seja a mais recente (necessária para suporte nativo ao driver MIPI-DSI e ao chip ESP32-P4):
```bash
pip install -U esphome
```

### 2. Configurar o Wi-Fi Local
Edite o arquivo `secrets.yaml` e defina os dados da sua rede inicial:
```yaml
wifi_ssid: "Nome_Da_Sua_Rede"
wifi_password: "Senha_Da_Sua_Rede"
```

### 3. Compilar e Gravar (Flash)
Execute o comando de gravação apontando para o arquivo de configuração correto:
```bash
esphome run display.yaml
```

**Durante a execução:**
1. O ESPHome compilará a aplicação (o primeiro build pode levar alguns minutos para construir os módulos do framework ESP-IDF).
2. Ao final do build, será perguntado qual interface de upload usar.
3. Conecte a placa via cabo USB ao computador, escolha a opção correspondente à porta COM detectada (ex: `COM4`) e digite o número correspondente.
4. Após o upload bem-sucedido, o terminal passará a exibir o log serial do display em tempo real.

---

## Hardware Suportado
- **Placa**: Guition JC8012P4A1C_I_W_Y (ESP32-P4 + ESP32-C6)
- **Display**: 10.1" MIPI-DSI IPS 1280x800 em modo landscape (horizontal)
- **Touchscreen**: GSL3680 via interface I2C (GPIO7 / GPIO8)

---

## Autor original
2º SGT PAULESKI / DA-DCT / Santa Maria-RS
