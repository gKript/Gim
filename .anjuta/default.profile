<?xml version="1.0"?>
<anjuta>
    <plugin name="Tools" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-tools:ATPPlugin"/>
    </plugin>
    <plugin name="Terminal" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-terminal:TerminalPlugin"/>
    </plugin>
    <plugin name="Code Snippets" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-snippets-manager:SnippetsManagerPlugin"/>
    </plugin>
    <plugin name="Subversion" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-subversion:Subversion"/>
    </plugin>
    <plugin name="Quick Open" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-quick-open:QuickOpenPlugin"/>
    </plugin>
</anjuta>
