static void launcher(const Arg *arg){
    int x, pos;
    unsigned int i;
    const char prompt[] = "EXEC: ";
    char tmp[32];
    char buf[256];
    Bool grabbing = True;
    KeySym ks;
    XEvent ev;

    memset(tmp, 0, sizeof(tmp));
    memset(buf, 0, sizeof(buf));
    pos = 0;

    x = 0;
    for(i = 0; i < LENGTH(tags); i++) x+= TEXTW(tags[i]);
    x += TEXTW(selmon->ltsymbol);
    dc.x = x;
    dc.w = selmon->ww - x - TEXTW(stext);

    XGrabKeyboard(dpy, ROOT, True, GrabModeAsync, GrabModeAsync, CurrentTime);

    drawtext(prompt, dc.norm, False);
    dc.x += TEXTW(prompt);
    XDrawLine(dpy, dc.drawable, dc.gc, dc.x, 3, dc.x, bh-3);

    XCopyArea(dpy, dc.drawable, selmon -> barwin, dc.gc, x, 0, dc.w, bh, x, 0);
    XSync(dpy, False);

    while(grabbing){
        if(ev.type == KeyPress) {
            XLookupString(&ev.xkey, tmp, sizeof(tmp), &ks, 0);

            switch(ks){
/*               case XK_Up:
                    if(launcher->nhisto)
                    {
                         if(histpos >= (int)launcher->nhisto)
                              histpos = 0;
                         strncpy(buf, launcher->histo[launcher->nhisto - ++histpos], sizeof(buf));
                         pos = strlen(buf);
                    }
                    break;
               case XK_Down:
                    if(launcher->nhisto && histpos > 0 && histpos < (int)launcher->nhisto)
                    {
                         strncpy(buf, launcher->histo[launcher->nhisto - --histpos], sizeof(buf));
                         pos = strlen(buf);
                    }
                    break;
               case XK_Return:
                    spawn("%s %s", launcher->command, buf);
                     //Histo 
                    if(launcher->nhisto + 1 > HISTOLEN)
                    {
                         for(i = launcher->nhisto - 1; i > 1; --i)
                              strncpy(launcher->histo[i], launcher->histo[i - 1], sizeof(launcher->histo[i]));

                         launcher->nhisto = 0;
                    }
                    // Store in histo array 
                    strncpy(launcher->histo[launcher->nhisto++], buf, sizeof(buf));

                    my_guitar_gently_wheeps = 0;
                    break;

               case XK_Escape:
                    my_guitar_gently_wheeps = 0;
                    break;
               case XK_Tab:
                    
                     * completion
                     * if there is not space in buffer we
                     * complete the command using complete_on_command.
                     * Else we try to complete on filename using
                     * complete_on_files.
                     
                    buf[pos] = '\0';
                    if (lastwastab)
                         tabhits++;
                    else
                    {
                         tabhits = 1;
                         strncpy(tmpbuf, buf, sizeof(tmpbuf));
                    }


                    if (pos)
                    {
                         if (strchr(tmpbuf, ' '))
                              complete = complete_on_files(tmpbuf, tabhits);
                         else
                              complete = complete_on_command(tmpbuf, tabhits);

                         if (complete)
                         {
                              strncpy(buf, tmpbuf, sizeof(buf));
                              strncat(buf, complete, sizeof(buf));
                              found = True;
                              free(complete);
                         }
                    }

                    lastwastab = True;

                     start a new round of tabbing 
                    if (found == False)
                         tabhits = 0;

                    pos = strlen(buf);

                    break;

               case XK_BackSpace:
                    lastwastab = False;
                    if(pos)
                         buf[--pos] = 0;
                    break;

               default:
                    lastwastab = False;
                    strncat(buf, tmp, sizeof(tmp));
                    ++pos;
                    break;
               }

               barwin_refresh_color(bw);

               // Update cursor position
               XSetForeground(dpy, gc, getcolor(infobar[selscreen].bar->fg));
               XDrawLine(dpy, bw->dr, gc,
                         1 + textw(launcher->prompt) + textw(" ") + textw(buf), 2,
                         1 + textw(launcher->prompt) + textw(" ") + textw(buf), INFOBARH - 4);

               barwin_draw_text(bw, 1, FHINFOBAR - 1, launcher->prompt);
               barwin_draw_text(bw, 1 + textw(launcher->prompt) + textw(" "), FHINFOBAR - 1, buf);
               barwin_refresh(bw);
          }
          else
               getevent(ev);
          XNextEvent(dpy, &ev);
     }

     barwin_unmap(bw);
     barwin_delete(bw);
     infobar_draw(selscreen);

     XUngrabKeyboard(dpy, CurrentTime);

     return;

}*/

                case XK_Return:
                    shexec(buf);
                    grabbing = False;
                    break;
                case XK_BackSpace:
                    if(pos) buf[--pos] = 0;
                    break;
                case XK_Escape:
                    grabbing = False;
                    break;
                default:
                    strncat(buf, tmp, sizeof(tmp));
                    ++pos;
                    break;
            }
        drawtext(buf, dc.norm, False);
        XDrawLine(dpy, dc.drawable, dc.gc, dc.x+TEXTW(buf), 3, dc.x+TEXTW(buf), bh-3);

        XCopyArea(dpy, dc.drawable, selmon->barwin, dc.gc, dc.x, 0, dc.w-TEXTW(prompt), bh, dc.x, 0);
        XSync(dpy, False);
        }
        XNextEvent(dpy, &ev);
    }

    drawbar(selmon);
    XUngrabKeyboard(dpy, CurrentTime);
    return;
}

pid_t shexec(const char *cmd){
    char *sh = NULL;
    pid_t pid;

    if(!(sh = getenv("SHELL"))) sh = "/bin/sh";

    if((pid = fork()) == 0){
        if(dpy) close(ConnectionNumber(dpy));

        setsid();
        execl(sh, sh, "-c", cmd, (char*)NULL);
    }

    return pid;
}

/*
static char *
complete_on_command(char *start, size_t hits)
{
     char *path;
     char *dirname;
     char *ret = NULL;
     DIR *dir;
     struct dirent *content;

     char **namelist = NULL;
     int n = 0, i;

     if (!getenv("PATH") || !start || hits <= 0)
         return NULL;

     path = xstrdup(getenv("PATH"));
     dirname = strtok(path, ":");

      recursively open PATH 
     while (dirname != NULL)
     {
          if ((dir = opendir(dirname)))
          {
               while ((content = readdir(dir)))
               {
                    if(strncmp(content->d_name, ".", 1))
                    {
                     if (!strncmp(content->d_name, start, strlen(start)))
                         {
                              namelist = xrealloc(namelist, ++n, sizeof(*namelist));
                              namelist[n-1] = xstrdup(content->d_name);
                         }
                    }
               }
               closedir(dir);
          }
          dirname = strtok(NULL, ":");
     }

     free(path);

     if(n > 0)
     {
          qsort(namelist, n, sizeof(char *), qsort_string_compare);
          ret = xstrdup(namelist[((hits > 0) ? hits - 1 : 0) % n] + strlen(start));

          for(i = 0; i < n; i++)
               free(namelist[i]);

          free(namelist);
     }

     return ret;

 * Complete a filename or directory name.
 * works like complete_on_command.
 
static char *
complete_on_files(char *start, size_t hits)
{
     char *ret = NULL;
     char *p = NULL;
     char *dirname = NULL;
     char *path = NULL;
     char *filepath = NULL;
     DIR *dir = NULL;
     struct dirent *content = NULL;
     struct stat st;
     size_t count = 0;


     if (!start || hits <= 0 || !(p = strrchr(start, ' ')))
          return NULL;

     
      * Search the directory to open and set
      * the beginning of file to complete on pointer 'p'.
      
     if (*(++p) == '\0' || !strrchr(p, '/'))
          path = xstrdup(".");
     else
     {
           remplace ~ by $HOME in dirname 
          if (!strncmp(p, "~/", 2) && getenv("HOME"))
               xasprintf(&dirname, "%s%s", getenv("HOME"), p+1);
          else
               dirname = xstrdup(p);

           Set p to filename to be complete
           * and path the directory containing the file
           * /foooooo/baaaaaar/somethinglikethis<tab>
           * <---- path - ---><------- p ------>
           
          p = strrchr(dirname, '/');
         if (p != dirname)
          {
               *(p++) = '\0';
               path = xstrdup(dirname);
          }
          else
          {
               path = xstrdup("/");
               p++;
          }
     }
     if ((dir = opendir(path)))
     {
          while ((content = readdir(dir)))
          {
               if (!strcmp(content->d_name, ".") || !strcmp(content->d_name, ".."))
                    continue;
               if (!strncmp(content->d_name, p, strlen(p)) && ++count == hits)
               {
                    // If it's a directory append '/' to the completion 
                    xasprintf(&filepath, "%s/%s", path, content->d_name);

                    if (filepath && stat(filepath, &st) != -1)
                    {
                         if (S_ISDIR(st.st_mode))
                              xasprintf(&ret, "%s/", content->d_name + strlen(p));
                         else
                              ret = xstrdup(content->d_name + strlen(p));
                    }
                    else
                         warn("%s", filepath);

                    free(filepath);

                    break;
               }
          }
          closedir(dir);
     }

     free(dirname);
     free(path);

     return ret;

*/
