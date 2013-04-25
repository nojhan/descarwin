%%
%% This is file `DESCARWINreport.cls',
%% THALES RESEARCH & TECHNOLOGY FRANCE
%% Computer Science Research Group, 
%%
\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{DESCARWINreport}
              [2010/03/01]
\typeout{****************************^^J%
         * DESCARWIN Technical Report  *^^J%
         * document class for LaTeX *^^J%
         * -TRT-FR / GR STI / LMTD- *^^J%
         ****************************}
\LoadClass[a4paper,10pt,twoside,onecolumn,final,openany]{report}

\RequirePackage[T1]{fontenc}
\RequirePackage[latin1]{inputenc}
\RequirePackage{lmodern}
\RequirePackage{lastpage}
\RequirePackage{fancyhdr}
\RequirePackage{graphicx}
\RequirePackage{babel}
\RequirePackage{ifthen}
\RequirePackage{multibib}
\RequirePackage{xcolor}
\newcites{mandatory}{Doc Mandatory}

\renewcommand\sfdefault{phv}
\renewcommand\familydefault{\sfdefault}

\definecolor{turquoise}{RGB}{93,191,212}

\RequirePackage{vmargin} 
\setpapersize{A4}
\setmargins{2.5cm}%			%left edge
					 {2.5cm}%     %top edge
           {17cm}%	  	%text width
           {22.42cm}%   %text hight
           {25pt}%			%header hight
           {0.5cm}%   	  %header distance 1
           {0pt}%				%footer hight
           {2.0cm}%    	%footer distance       1.5   


\newif\if@francais \@francaisfalse
\DeclareOption{francais}{\@francaistrue%
        \gdef\@documenttype{DESCARWIN ANR-09-COSI-002}%
        \gdef\@labquery{groupe / laboratoire}%
        \gdef\@docrefquery{num\'ero document}%
        \gdef\@revisionquery{r\'evision}%
        \PassOptionsToPackage{francais}{babel}
        \gdef\@disclamer{Ce document et les informations qu'il contient sont la propri\'et\'e de THALES, INRIA and ONERA. Ils ne peuvent \^etre reproduits, communiqu\'es ou utilis\'es sans son autorisation \'ecrite pr\'ealable}
        \gdef\@purposetitle{Objet}
        \gdef\@areferencetitle{\`A appliquer}
        \gdef\@rreferencetitle{De référence}
        \gdef\@revisiontitle{suivi des évolutions}
        \gdef\@irevlabtabletitle{Indices de révision}
        \gdef\@mrevlabtabletitle{Modifications}
        \gdef\@authorstitle{\'Etabli par}
        \gdef\@approverstitle{Approuvé par}
        \gdef\@dateslabel{\ le :\ }
        }
\DeclareOption{english}{%
        \gdef\@documenttype{DESCARWIN ANR-09-COSI-002}%
        \gdef\@labquery{group / laboratory}%
        \gdef\@docrefquery{document number}%
        \gdef\@revisionquery{revision}%
        \PassOptionsToPackage{english}{babel}
        \gdef\@disclamer{The information contained in this document and any attachments are the property of THALES, INRIA and ONERA. You are hereby notified that any review, dissemination, distribution, copying or otherwise use of this document is strictly prohibited without THALES, INRIA and ONERA prior written approval.}
        \gdef\@purposetitle{Purpose}
        \gdef\@areferencetitle{Mandatory}
        \gdef\@rreferencetitle{Reference}
        \gdef\@revisiontitle{document control}
        \gdef\@irevlabtabletitle{Revision index}
        \gdef\@mrevlabtabletitle{Modifications}
        \gdef\@authorstitle{Written by}
        \gdef\@approverstitle{Approved by}
        \gdef\@dateslabel{:\ }
}
\ProcessOptions
          
\newcommand{\laboratory}[1]{\gdef\@laboratory{#1}}
\newcommand{\revision}[1]{\gdef\@revision{#1}}
\newcommand{\docref}[1]{\gdef\@docref{#1}}
\gdef\@laboratory{Gxxx / Lyyy}
\gdef\@revision{--}
\gdef\@docref{XX XXX XXX-XXX}

%%%%%%%%%%%
%% HEADERS AND FOOTERS
\renewcommand{\headrulewidth}{0.8pt}
\renewcommand{\footrulewidth}{0.8pt}
\lhead[]%
      {DESCARWIN ANR-09-COSI-002}
\rhead[DESCARWIN ANR-09-COSI-002]%
      {}
\lfoot[\small \thepage/\pageref{LastPage}]%
      {\textbf{\small TRT - INRIA - ONERA}}
\cfoot[{\large \@docref} \hspace{2cm}\@revision]%
      {\hspace{2cm}{\large \@docref} \hspace{2cm} \@revision}
\rfoot[\textbf{\small TRT - INRIA - ONERA}]%
      {\small \thepage/\pageref{LastPage}}
%%%%%%%%%%%%%
%% TITLEPAGE
  \if@titlepage
  \renewcommand\maketitle{
\begin{titlepage}%
  %\let\footnotesize\small
  \let\footnoterule\relax
  \let \footnote \thanks

	\vspace*{-2cm} 
	\hspace*{-1,3cm} 
	\setlength{\unitlength}{1pt}
		\color{turquoise}
	  \begin{picture}(0,0)(10,38)
	  	\multiput(0,10)(0.1,0){75}{\line(0,0){61}}
	  \end{picture}
		\color{black}
	  \vspace*{-1cm} 
		\begin{tabular}{cp{16cm}}
			\multicolumn{2}{l}{\ \textbf{\large TRT - INRIA - ONERA}}\\
			\vspace{0,5cm}&\\%
			\multicolumn{2}{l}{\ \huge \textbf{ANR-09-COSI-002 Technical Report}} \\%
		\end{tabular}

	  \begin{picture}(0,0)(23,29.5)
	  	\multiput(0,0)(0,0.5){2}{\line(1,0){500}}
	  \end{picture}

%	\vfill
	\vspace{5cm} 
	\begin{center}\Huge \bfseries \@title \\
\vfill
\includegraphics[width=.3\textwidth]{hd_logo_thales}
\hfill
\includegraphics[width=.25\textwidth]{INRIA_logo}
\hfill
\includegraphics[width=.3\textwidth]{ONERA_logo}\end{center}
	%\vfill
	
	\hspace{-2cm}\vspace*{-2,5cm}
	\begin{tabular}{|p{7.6cm}|p{8.1cm}|c|}
		\multicolumn{3}{p{18.4cm}}{\centering\scriptsize{\@disclamer}}\\ \hline
		\centering\textsc{\@labquery} & \centering\textsc{\@docrefquery} & \hspace{7mm}\textsc{page}\hspace{7mm} \\	
		\hline
		\centering\normalsize\textsc{\@laboratory} & \centering\Large\textsc{\@docref}    & \small \thepage/\pageref{LastPage} \rule[-7pt]{0pt}{20pt}\\ \hline
		\multicolumn{2}{|r|}{\large \textbf{\@revision}} & \textsc{\@revisionquery}  \rule[-7pt]{0pt}{20pt} \\ \hline
	\end{tabular}
  \@thanks
  \vfil\null
  \end{titlepage}%
  \setcounter{footnote}{0}%
	\global\let\thanks\relax
  \global\let\maketitle\relax
  \global\let\@thanks\@empty
  \global\let\@author\@empty
  \global\let\@date\@empty
  \global\let\@title\@empty
  \global\let\title\relax
  \global\let\author\relax
  \global\let\date\relax
  \global\let\and\relax
  \global\let\docref\relax
  \global\let\revision\relax
  \global\let\laboratory\relax
}

%%%%%%%
%% CHAPTER PAGE STYLE
\renewcommand*\chaptermark[1]{}
\renewcommand\chapter{
                    \global\@topnum\z@
                    \@afterindentfalse
                    \secdef\@chapter\@schapter}
\def\@makechapterhead#1{%
  \vspace*{30\p@}%
  {\parindent \z@ \raggedright \normalfont
    \ifnum \c@secnumdepth >\m@ne
        \large\bfseries
        \thechapter.\space
    \fi
    \interlinepenalty\@M
    \large \MakeUppercase{#1}\par\nobreak
    \vskip 20\p@
  }}

\def\@makeschapterhead#1{%
  \vspace*{30\p@}%
  {\parindent \z@ \raggedright
    \normalfont
    \interlinepenalty\@M
    \centering \Large \MakeUppercase{#1} \par\nobreak
    \vskip 20\p@
  }}

\renewcommand\section{\@startsection {section}{1}{\z@}%
                                   {-3.5ex \@plus -1ex \@minus -.2ex}%
                                   {2.3ex \@plus.2ex}%
                                   {\normalfont\large\bfseries\MakeUppercase}}
\renewcommand\subsection{\@startsection{subsection}{2}{\z@}%
                                     {-3.25ex\@plus -1ex \@minus -.2ex}%
                                     {1.5ex \@plus .2ex}%
                                     {\normalfont\normalsize\bfseries\MakeUppercase}}
\renewcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
                                     {-3.25ex\@plus -1ex \@minus -.2ex}%
                                     {1.5ex \@plus .2ex}%
                                     {\normalfont\normalsize\bfseries\MakeUppercase}}


%%%%%%
%% BIBLIOGRAPHY PAGE STYLE
\renewenvironment{thebibliography}[1]
     {\@mkboth{\MakeUppercase\bibname}{\MakeUppercase\bibname}%
      \list{\@biblabel{\@arabic\c@enumiv}}%
           {\settowidth\labelwidth{\@biblabel{#1}}%
            \leftmargin\labelwidth
            \advance\leftmargin\labelsep
            \@openbib@code
            \usecounter{enumiv}%
            \let\p@enumiv\@empty
            \renewcommand\theenumiv{\@arabic\c@enumiv}}%
      \sloppy
      \clubpenalty4000
      \@clubpenalty \clubpenalty
      \widowpenalty4000%
      \sfcode`\.\@m}
     {\def\@noitemerr
       {\@latex@warning{Empty `thebibliography' environment}}%
      \endlist}


%%%%%%%%
%% ABSTRACT IS SUBJECT
\if@titlepage
  \renewenvironment{abstract}{%
        \newpage
        \chapter{\@purposetitle}
        }%
     {}
\else
  \renewenvironment{abstract}{%
      \if@twocolumn
        \section*{\abstractname}%
      \else
        \small
        \begin{center}%
          {\bfseries \abstractname\vspace{-.5em}\vspace{\z@}}%
        \end{center}%
        \quotation
      \fi}
      {\if@twocolumn\else\endquotation\fi}
\fi

%%%%%%%%
%% BIBLIO REF ENVIRONNEMENTS
\newcommand{\references}[2]{%
\chapter{Documents}%
	\section{\@areferencetitle}%
	  \nocitemandatory{*}
	  \ifthenelse{\equal{#1}{}}%
	     {\textit{\large --nihil--}}%
	     {\bibliographystylemandatory{unsrt}\bibliographymandatory{#1}}
	\section{\@rreferencetitle}%
		\ifthenelse{\equal{#2}{}}%
		   {\textit{\large --nihil--}}%
		   {\bibliographystyle{unsrt}\bibliography{#2}}%
%
%\newpage%
}

%%%%%%%%
%% REVISION FOLLOW UP
\newcounter{revisionvalue}
\newcounter{revlabvalue}
\newenvironment{revisions}{%
     \setcounter{revisionvalue}{0}
     \setcounter{revlabvalue}{0}

     \begin{center}
     		\vspace*{0.3cm}
        \textbf{{\Large \MakeUppercase{\@revisiontitle}}}
      	\vspace{2,5cm}

}{\end{center}}

\newenvironment{revtable}{
	      %\hspace{-1cm}
	      \begin{tabular}{|p{2,3cm}|p{3,1cm}|p{3,1cm}|p{3,1cm}|p{3,1cm}|}
		         \hline
}{  	 \hline
		\end{tabular}
}

\newenvironment{revisionlabels}{%
	\begin{center}
  \addtocounter{revisionvalue}{-1}
	\vspace{0,5cm}
	%\hspace{-1cm}
	\begin{tabular}{|p{3cm}|p{13,4cm}|}
		\hline
		{\@irevlabtabletitle} & \hspace{5,5cm}\@mrevlabtabletitle \rule[-7pt]{0pt}{20pt} \\ \hline
}
{
   \whiledo{\value{revlabvalue} < \value{revisionvalue}}{\revlabel{}} \revisionvaluetext{revlabvalue} & \rule[-7pt]{0pt}{20pt} \\ \hline
	\end{tabular}
	\stepcounter{revisionvalue}
\end{center}
}
%%%{label}
\newcommand{\revlabel}[1]{\ \revisionvaluetext{revlabvalue} & #1 \rule[-7pt]{0pt}{20pt} \\ \hline }

\newcommand{\dates}[4]{ &%
     \revisionvaluetext{revisionvalue}\textedate#1 &%
     \revisionvaluetext{revisionvalue}\textedate#2 &% 
     \revisionvaluetext{revisionvalue}\textedate#3 &% 
     \revisionvaluetext{revisionvalue}\textedate#4 \rule[-7pt]{0pt}{20pt}\\ \hline}

\newcommand{\writers}[4]{%
     \@authorstitle &%
     \names{#1} & \names{#2} & \names{#3} & \names{#4}\\[0,5cm]%
     Signature& & & & \\[1cm]}

\newcommand{\approvers}[4]{\hline%
     \@approverstitle &%
     \names{#1} & \names{#2} & \names{#3} & \names{#4}\\[0,5cm]%
     Signature& & & & \\[1cm]}

\newcommand{\textedate}{\@dateslabel}
\newcommand{\names}[1]{\hspace*{-4.5pt}\begin{tabular}{p{3,1cm}}\rule[0pt]{0pt}{13pt}#1 \end{tabular}}
\newcommand{\revisionvaluetext}[1]{\ifthenelse{\value{#1}=0}{--}{\Alph{#1}}\stepcounter{#1}}
%%%%%%%%%%%%%%%
%%% TABLE OF CONTENTS
\renewcommand\tableofcontents{%
		\newpage
    \if@twocolumn
      \@restonecoltrue\onecolumn
    \else
      \@restonecolfalse
    \fi
    \chapter*{\contentsname
        \@mkboth{%
           \MakeUppercase\contentsname}{\MakeUppercase\contentsname}}%
    \@starttoc{toc}%
    \if@restonecol\twocolumn\fi
    }

\setcounter{page}{2}
\pagestyle{fancy}
