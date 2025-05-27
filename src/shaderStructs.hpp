#ifndef SHADERSTRUCTS_HPP
#define SHADERSTRUCTS_HPP

// utilites for writing shader setters

// for VAO attributes
#define STRUCT_MEMBER_ATTRIB(attrNum, structName, member) \
	do { \
		glEnableVertexAttribArray(attrNum); \
		glVertexAttribPointer(attrNum, sizeof(structName::member) / sizeof(float), GL_FLOAT, \
		                      GL_FALSE, sizeof(structName), (void*)offsetof(structName, member)); \
	} while (false)

// makes writing shader setters easier
#define SET_UNIFORM_ATTR(attr) \
	do { \
		if (index == NOT_ARRAY) { \
			shader.setUniform(uniformName + "." + #attr, value.attr); \
		} else { \
			shader.setUniform(uniformName + "[" + std::to_string(index) + "]." + #attr, \
			                  value.attr); \
		} \
	} while (false)

// Specifies the value is not an array but a single value. For uniform setters.
#define NOT_ARRAY -1

#endif /* SHADERSTRUCTS_HPP */
