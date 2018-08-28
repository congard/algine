/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330 core
in vec4 fragPos; // world space

uniform vec3 lightPos; // world space
uniform float far_plane; // shadow matrix far plane

void main()
{
    float lightDistance = length(fragPos.xyz - lightPos);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}
