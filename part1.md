## Resolución de la primera parte

Se editó únicamente el archivo `exec.c` para cumplir con la funcionalidad pedida en la primera parte.
Se muestra un diff de la función modificada, `exec_cmd`:
```diff
                case EXEC:
                        // spawns a command
                        //
-                       // Your code here
-                       printf("Commands are not yet implemented\n");
+                       handle_exec(cmd);
                        _exit(-1);
                        break;
```

```C
void handle_exec(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;
    for (int i = 0; i < execcmd->argc; ++i) {
        if (execcmd->argv[i][0] == '$') {
            execcmd->argv[i] = getenv(execcmd->argv[i] + 1);
        }
    }

    execvp(execcmd->argv[0], execcmd->argv);
}
```