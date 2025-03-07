#version 330 core

out float FragDepth;  // Sortie de la profondeur

void main() {
    // La sortie du fragment est la profondeur du fragment dans l'espace de la lumière
    FragDepth = gl_FragCoord.z;
}
