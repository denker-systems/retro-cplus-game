/**
 * @file IPropertyEditor.h
 * @brief Interface för property editors
 */
#pragma once

#include <string>

/**
 * @brief Interface för alla property editors
 * 
 * Varje datatyp (Room, Dialog, Quest, etc) har sin egen PropertyEditor
 * som implementerar detta interface.
 */
class IPropertyEditor {
public:
    virtual ~IPropertyEditor() = default;
    
    /**
     * @brief Rendera UI för denna property type
     */
    virtual void render() = 0;
    
    /**
     * @brief Validera data innan save
     * @param outError Felmeddelande om validering misslyckas
     * @return true om data är valid
     */
    virtual bool validate(std::string& outError) const = 0;
    
    /**
     * @brief Är data ändrad sedan senaste save?
     */
    virtual bool isDirty() const = 0;
    
    /**
     * @brief Återställ till senast sparade state
     */
    virtual void revert() = 0;
    
    /**
     * @brief Applicera ändringar (markera som sparad)
     */
    virtual void apply() = 0;
};
