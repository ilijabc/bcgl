varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texCoord;
varying vec4 v_color;

#ifdef VERTEX
void main()
{
    v_position = (u_ModelViewMatrix * vec4(a_Position, 1)).xyz;
    v_normal = (u_ModelViewMatrix * vec4(a_Normal, 0)).xyz;
    v_texCoord = a_TexCoord;
    v_color = a_Color;
    gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4(a_Position, 1);
}
#endif

#ifdef FRAGMENT
void main()
{
    gl_FragColor = u_ObjectColor;
    // texture
    if (u_UseTexture)
    {
        vec4 tex = texture2D(u_Texture, v_texCoord);
        if (u_AlphaTest && tex.a < 0.1)
            discard;
        if (u_AlphaOnlyTexture)
            tex = vec4(1, 1, 1, tex.a);
        gl_FragColor *= tex;
    }
    // lighting
    if (u_LightEnabled)
    {
        vec3 norm = normalize(v_normal);
        // vec3 lightPos = (u_CameraMatrix * vec4(u_LightPosition, 1)).xyz;
        vec3 lightDir = normalize(u_LightPosition - v_position);
        float diff = max(dot(norm, lightDir), 0.0);
        vec4 diffuse = diff * u_LightColor * u_DiffuseColor;
        gl_FragColor *= (u_AmbientColor + diffuse);
    }
    // vertex color
    if (u_VertexColorEnabled)
    {
        gl_FragColor *= v_color;
    }
}
#endif
