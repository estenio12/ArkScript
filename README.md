# 🛸 ArkScript
> **A Arca Segura do Desenvolvimento Web.** Unindo a robustez do Rust à produtividade e ergonomia do TypeScript.

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Compiler](https://img.shields.io/badge/compiler-C%2B%2B17-00599C.svg)](#-arquitetura-do-compilador)
[![Target](https://img.shields.io/badge/target-JavaScript%20%2F%20Node.js-yellow.svg)](#-compatibilidade-e-ecossistema)

---

## 📌 Visão Geral

Se você desenvolve para a Web hoje, provavelmente enfrenta um dilema constante:

- **TypeScript** oferece excelente produtividade e DevEx, mas a perda do tipo em tempo de execução (*type erasure*) permite que exceções não tratadas como `TypeError: Cannot read properties of undefined` cheguem à produção.
- **Rust** entrega garantia contra *crashes* e máxima segurança, mas impõe uma curva de aprendizado íngreme e um ciclo de desenvolvimento mais lento devido à complexidade do *Borrow Checker*.

A **ArkScript** foi criada para resolver essa dicotomia. É uma linguagem de paradigma **imperativo-estruturado** projetada para garantir **resiliência total em produção** com uma **experiência de desenvolvimento limpa e pragmática**, compilando nativamente para código JavaScript altamente otimizado (**JSSafe**).

---

## 🎯 Objetivos do Projeto

Desenvolvida com base em desafios reais do mercado de software, a ArkScript foca em quatro pilares fundamentais de engenharia e negócios:

1. **Robustez Absoluta (*"Compilou, não quebra"*)**  
   Eliminação categórica de exceções não tratadas em tempo de execução (*runtime crashes*), garantindo previsibilidade para sistemas críticos.

2. **Ergonomia e Produtividade (DevEx)**  
   Sintaxe familiar e intuitiva. O desenvolvedor foca na regra de negócio e no valor entregue ao cliente, em vez de disputar com o compilador.

3. **Compatibilidade com o Ecossistema NPM**  
   Aproveite o ecossistema Web (React, Angular, Vue, Node.js) sem herdar a fragilidade histórica de tipagem do JavaScript.

4. **Desempenho e Eficiência de Execução**  
   Geração de JavaScript estruturado e plano, projetado para maximizar otimizações dos motores JIT (como o V8) e garantir a eficiência de cache do processador.

---

## 🛠️ Decisões de Arquitetura & Recursos da Linguagem

Para entregar alta confiabilidade sem sacrificar a velocidade de escrita, a ArkScript adota garantias de segurança diretamente em seu compilador — construído em **C++**:

### 1. Ausência de `null` e `undefined` via Valores Padrão (*Defaults*)
A ArkScript elimina o conceito de valores nulos. Caso um dado primitivo não seja inicializado explicitamente, ele assume automaticamente um valor seguro do seu tipo (ex: `""`, `0`, `false`).

### 2. Construtores Obrigatórios (`::new()`)
A instanciação de estruturas de dados (*structs*) exige o preenchimento de todos os campos obrigatórios. É impossível instanciar objetos parciais ou inconsistentes na memória.

### 3. Encapsulamento Restritivo por Padrão (`pub`)
Seguindo o **Princípio do Menor Privilégio**, membros e módulos são privados por padrão. A exposição explícita ocorre apenas através da palavra-chave `pub`, prevenindo vazamentos acidentais de arquitetura.

### 4. Herança Múltipla Pragmática (Operador `!`)
A composição é realizada por cópia estrutural de propriedades em tempo de compilação. Para resolução explícita de conflitos de propriedades ou tipos, utiliza-se o operador de prioridade `!`:

```arkscript
// O operador '!' define precedência explícita na herança estrutural
struct Dog extends Wolf, !PetSheet