% This document provides the tech memo class for use on Michigan
% Exploration Laboratory documents
%
% Author: K. Scott Tripp

%***** Template version log *****************************************
% 9 February 2012	- Initial draft		- KST
% 14 June 2012 		- Draft release 	- KST
% 12 July 2012	 	- Initial release	- KST
%********************************************************************
\newcommand{\templateRev}{1}
\newcommand{\revDate}{12 July 2012}

% Class Definition
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{mxl-memo}[2012/02/09 - A Technical Memo class for MXL]


% Based off the Article class
\LoadClassWithOptions{article}

% Package requirements
\usepackage{fullpage}
\usepackage{graphicx}
\usepackage{fancyhdr}
\RequirePackage{ifthen}

% Memo commands
\renewcommand{\to}[1]{\gdef\@to{#1}}
\def\@to{\relax}

\renewcommand{\author}[1]{\gdef\@author{#1}}
\def\@author{\relax}

\renewcommand{\title}[1]{\gdef\@title{#1}}
\def\@title{\relax}

\newcommand{\docnum}[1]{\gdef\@docnum{#1}}
\def\@docnum{\relax}


% TODO: define revision entries
% Goal: enter date, name/initials, and comments in the form of:
% \revision{0.1}{Initial release}{KST}
% Use the highest revision number as the document revision
\newcommand{\revision}[3]{
	\gdef\@revision{#1}
	\gdef\@comment{#2}
	\gdef\@initials{#3}
}

% Geometry variables
\newcommand{\TableUnindent}{-0.2cm}

%===Document formatting==============================================

% Title block
\renewcommand{\maketitle}{\makeTitleBlock}
\newcommand\makeTitleBlock{

% Geometry definitions
\newcommand{\blockWidth}{0.79\textwidth}

%\newcommand{\HRule}{\rule{1.4\textwidth}{0.5mm}}
\newcommand{\HRule}{\rule{1.0\textwidth}{0.5mm}}

% Banner and Title Block
\begin{flushleft}
\begin{minipage}{\blockWidth}	% outer box
\begin{minipage}[t]{0.78\textwidth}	% banner
	%\raisebox{0.5in}{\Huge \sc MXL Tech Memo}
	{\Huge MXL Tech Memo}\\ 
	\HRule \\
	{\small Document \@docnum}\\[0.1in]
\end{minipage}
\begin{minipage}[b]{0.78\textwidth}	% title block
	\begin{tabular}{l l}
		\hspace{\TableUnindent}{\bf To:} & {\@to \par}\\[0.05in]
		\hspace{\TableUnindent}{\bf Authors:} &{\@author \par}\\[0.05in]
		\hspace{\TableUnindent}{\bf Date:} &{\today}\\[0.05in]
		\hspace{\TableUnindent}{\bf Subject:} &{\@title \par}\\
	\end{tabular}
\end{minipage}
\end{minipage}
\begin{minipage}{0.2\textwidth}
	\begin{flushright}
	\includegraphics[width=1.2in]{Dish-Logo-4.jpg}
	\end{flushright}
\end{minipage}
\end{flushleft}

}

% TODO: Define revision history environment here

%**** Define page headers. ******************************************

\renewcommand{\footrulewidth}{0.4pt}% default is 0pt

% First page

\fancypagestyle{firstPage}
{
	\fancyhf{}
	\renewcommand{\headrulewidth}{0pt}% default is 0.4pt
	\renewcommand{\headheight}{0pt}% 
	\renewcommand{\topmargin}{-25pt}% 
	\fancyfoot[L]{Template version \templateRev}
	\fancyfoot[C]{MXL Tech Memo}
	\fancyfoot[R]{Page \thepage}
}

\thispagestyle{firstPage}
%\fancyhf{}
%
%\lfoot{\today}
%\cfoot{MXL Tech Memo}
%\rfoot{Template version \templateRev}

% Remaining pages
\fancyhf{}
\pagestyle{fancy}
\headsep 25pt

\lhead{\@title}
\chead{Document \@docnum}
\rhead{\today}

\lfoot{Template version \templateRev}
\cfoot{MXL Tech Memo}
\rfoot{Page \thepage}
