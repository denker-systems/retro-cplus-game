# Generella Regler

<communication>
- Kommunicera på svenska
- Förklara koncept pedagogiskt
</communication>

<code_style>
- C++17 standard
- Modern C++ idiom (smart pointers, RAII)
- Tydliga klassnamn (PascalCase)
- Medlemsvariabler med m_ prefix
- Metoder i camelCase
</code_style>

<architecture>
- Game-klassen är central koordinator
- Varje system (Input, Renderer, etc.) är separat klass
- Använd unique_ptr för ägande
- SDL-resurser wrappas i klasser
</architecture>
