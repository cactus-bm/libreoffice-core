#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#
import uno
import traceback
from ..text.TextElement import TextElement
from ..text.TextDocument import TextDocument
from ..common.FileAccess import FileAccess
from ..text.TextSectionHandler import TextSectionHandler
from ..common.Helper import Helper

from datetime import date as dateTimeObject

from com.sun.star.text.PlaceholderType import TEXT
from com.sun.star.i18n.NumberFormatIndex import TIME_HHMM, DATE_SYSTEM_LONG

'''
The classes here implement the whole document-functionality of the agenda wizard:
the live-preview and the final "creation" of the document,
when the user clicks "finish". <br/>
<br/>
<h2>Some terminology:<h2/>
items are names or headings. we don't make any distinction.

<br/>
The Agenda Template is used as general "controller"
of the whole document, whereas the two child-classes ItemsTable
and TopicsTable control the item tables (note plural!) and the
topics table (note singular).<br/>
<br/>
Other small classes are used to abstract the handling of cells and text and we
try to use them as components.
<br/><br/>
We tried to keep the Agenda Template as flexible as possible, though there
must be many limitations, because it is generated dynamically.<br/><br/>
To keep the template flexible the following decisions were made:<br/>
1. Item tables.<br/>
1.a. there might be arbitrary number of Item tables.<br/>
1.b. Item tables design (bordewr, background) is arbitrary.<br/>
1.c. Items text styles are individual,
and use stylelist styles with predefined names.<br/>
As result the following limitations:<br/>
Pairs of Name->value for each item.<br/>
Tables contain *only* those pairs.<br/>
2. Topics table.<br/>
2.a. arbitrary structure.<br/>
2.b. design is arbitrary.<br/>
As result the following limitations:<br/>
No column merge is allowed.<br/>
One compolsary Heading row.<br/>
<br/><br/>
To let the template be flexible, we use a kind of "detection": we look where
the items are read the design of each table, reaplying it after writing the
table.self.xTextDocument
<br/><br/>
A note about threads:<br/>
Many methods here are synchronized, in order to avoid colission made by
events fired too often.
@author rpiterman
'''
class AgendaTemplate(TextDocument):

    '''
    constructor. The document is *not* loaded here.
    only some formal members are set.
    '''

    def __init__(self,  xmsf, agenda, resources, templateConsts, listener):
        super(AgendaTemplate,self).__init__(xmsf,listener, None,
            "WIZARD_LIVE_PREVIEW")
        self.agenda = agenda
        self.templateConsts = templateConsts
        self.resources = resources
        self.itemsMap = {}
        self.allItems = []

    def load(self, templateURL, topics):
        self.template = self.calcTemplateName(templateURL)
        self.loadAsPreview(templateURL, False)
        self.xFrame.ComponentWindow.Enable = False
        self.xTextDocument.lockControllers()
        self.initialize()
        self.initializeData(topics)
        self.xTextDocument.unlockControllers()

    '''
    The agenda templates are in format of aw-XXX.ott
    the templates name is then XXX.ott.
    This method calculates it.
    '''

    def calcTemplateName(self, url):
        return FileAccess.connectURLs(
            FileAccess.getParentDir(url), FileAccess.getFilename(url)[3:])

    '''synchronize the document to the model.<br/>
    this method rewrites all titles, item tables , and the topics table-
    thus synchronizing the document to the data model (CGAgenda).
    information (it is only actualized on save) the given list
    supplies this information.
    '''

    def initializeData(self, topicsData):
        for i in self.itemsTables:
            try:
                i.write("")
            except Exception:
                traceback.print_exc()

        self.redrawTitle("txtTitle")
        self.redrawTitle("txtDate")
        self.redrawTitle("txtTime")
        self.redrawTitle("cbLocation")

    '''
    redraws/rewrites the table which contains the given item
    This method is called when the user checks/unchecks an item.
    The table is being found, in which the item is, and redrawn.
    '''

    def redraw(self, itemName):
        self.xTextDocument.lockControllers()
        try:
            # get the table in which the item is...
            itemsTable = self.itemsMap[itemName]
            # rewrite the table.
            itemsTable.write(None)
        except Exception:
            traceback.print_exc()
        self.xTextDocument.unlockControllers()

    '''
    checks the data model if the
    item corresponding to the given string should be shown
    '''

    def isShowItem(self, itemName):
        if itemName == self.templateConsts.FILLIN_MEETING_TYPE:
            return self.agenda.cp_ShowMeetingType
        elif itemName == self.templateConsts.FILLIN_READ:
            return self.agenda.cp_ShowRead
        elif itemName == self.templateConsts.FILLIN_BRING:
            return self.agenda.cp_ShowBring
        elif itemName == self.templateConsts.FILLIN_NOTES:
            return self.agenda.cp_ShowNotes
        elif itemName == self.templateConsts.FILLIN_FACILITATOR:
            return self.agenda.cp_ShowFacilitator
        elif itemName == self.templateConsts.FILLIN_TIMEKEEPER:
            return self.agenda.cp_ShowTimekeeper
        elif itemName == self.templateConsts.FILLIN_NOTETAKER:
            return self.agenda.cp_ShowNotetaker
        elif itemName == self.templateConsts.FILLIN_PARTICIPANTS:
            return self.agenda.cp_ShowAttendees
        elif itemName == self.templateConsts.FILLIN_CALLED_BY:
            return self.agenda.cp_ShowCalledBy
        elif itemName == self.templateConsts.FILLIN_OBSERVERS:
            return self.agenda.cp_ShowObservers
        elif itemName == self.templateConsts.FILLIN_RESOURCE_PERSONS:
            return self.agenda.cp_ShowResourcePersons
        else:
            raise ValueError("No such item")

    '''itemsCache is a Map containing all agenda item. These are object which
    "write themselfs" to the table, given a table cursor.
    A cache is used in order to reuse the objects, instead of recreate them.
    This method fills the cache will all items objects (names and headings).
    '''

    def initItemsCache(self):
        self.itemsCache = {}
        # Headings
        self.itemsCache[
                self.templateConsts.FILLIN_MEETING_TYPE] = \
            AgendaItem(self.templateConsts.FILLIN_MEETING_TYPE,
                self.resources.itemMeetingType,
                PlaceholderElement(
                    self.resources.reschkMeetingTitle_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))
        self.itemsCache[
                self.templateConsts.FILLIN_BRING] = \
            AgendaItem(self.templateConsts.FILLIN_BRING,
                self.resources.itemBring,
                PlaceholderElement (
                    self.resources.reschkBring_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))
        self.itemsCache[
                self.templateConsts.FILLIN_READ] = \
            AgendaItem (self.templateConsts.FILLIN_READ, 
                self.resources.itemRead,
                PlaceholderElement (
                    self.resources.reschkRead_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))
        self.itemsCache[
                self.templateConsts.FILLIN_NOTES] = \
            AgendaItem (self.templateConsts.FILLIN_NOTES,
                self.resources.itemNote,
                PlaceholderElement (
                    self.resources.reschkNotes_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))

        # Names
        self.itemsCache[
                self.templateConsts.FILLIN_CALLED_BY] = \
            AgendaItem(self.templateConsts.FILLIN_CALLED_BY,
                self.resources.itemCalledBy,
                PlaceholderElement (
                    self.resources.reschkConvenedBy_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))
        self.itemsCache[
                self.templateConsts.FILLIN_FACILITATOR] = \
            AgendaItem(self.templateConsts.FILLIN_FACILITATOR,
                self.resources.itemFacilitator,
                PlaceholderElement (
                    self.resources.reschkPresiding_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))
        self.itemsCache[
                self.templateConsts.FILLIN_PARTICIPANTS] = \
            AgendaItem(self.templateConsts.FILLIN_PARTICIPANTS,
                self.resources.itemAttendees,
                PlaceholderElement(
                    self.resources.reschkAttendees_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))
        self.itemsCache[
                self.templateConsts.FILLIN_NOTETAKER] = \
            AgendaItem(self.templateConsts.FILLIN_NOTETAKER,
                self.resources.itemNotetaker,
                PlaceholderElement(
                    self.resources.reschkNoteTaker_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))
        self.itemsCache[
                self.templateConsts.FILLIN_TIMEKEEPER] = \
            AgendaItem(self.templateConsts.FILLIN_TIMEKEEPER,
                self.resources.itemTimekeeper,
                PlaceholderElement(
                    self.resources.reschkTimekeeper_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))
        self.itemsCache[
                self.templateConsts.FILLIN_OBSERVERS] = \
            AgendaItem(self.templateConsts.FILLIN_OBSERVERS,
                self.resources.itemObservers,
                PlaceholderElement(
                    self.resources.reschkObservers_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))
        self.itemsCache[
                self.templateConsts.FILLIN_RESOURCE_PERSONS] = \
            AgendaItem(self.templateConsts.FILLIN_RESOURCE_PERSONS,
                self.resources.itemResource,
                PlaceholderElement(
                    self.resources.reschkResourcePersons_value,
                    self.resources.resPlaceHolderHint, self.xTextDocument))

    '''Initializes a template.<br/>
    This method does the following tasks:<br/>
    Get a Time and Date format for the document, and retrieve the null
    date of the document (which is document-specific).<br/>
    Initializes the Items Cache map.
    Analyses the document:<br/>
    -find all "fille-ins" (apear as &gt;xxx&lt; in the document).
    -analyze all items sections (and the tables in them).
    -locate the titles and actualize them
    -analyze the topics table
    '''

    def initialize(self):
        '''
        Get the default locale of the document,
        and create the date and time formatters.
        '''
        AgendaTemplate.dateUtils = Helper.DateUtils(
            self.xMSF, self.xTextDocument)
        AgendaTemplate.formatter = AgendaTemplate.dateUtils.formatter
        AgendaTemplate.dateFormat = \
            AgendaTemplate.dateUtils.getFormat(DATE_SYSTEM_LONG)
        AgendaTemplate.timeFormat = \
            AgendaTemplate.dateUtils.getFormat(TIME_HHMM)

        self.initItemsCache()
        self.allItems = self.searchFillInItems(0)
        self.initializeTitles()
        self.initializeItemsSections()
        self.textSectionHandler = TextSectionHandler(
            self.xTextDocument, self.xTextDocument)
        self.topics = Topics(self)

    '''
    locates the titles (name, location, date, time)
    and saves a reference to thier Text ranges.
    '''

    def initializeTitles(self):
        auxList = []
        for i in self.allItems:
            text = i.String.lstrip().lower()
            if text == self.templateConsts.FILLIN_TITLE:
                AgendaTemplate.teTitle = PlaceholderTextElement(
                    i, self.resources.resPlaceHolderTitle,
                    self.resources.resPlaceHolderHint,
                    self.xTextDocument)
                AgendaTemplate.trTitle = i
            elif text == self.templateConsts.FILLIN_DATE:
                AgendaTemplate.teDate = PlaceholderTextElement(
                    i, self.resources.resPlaceHolderDate,
                    self.resources.resPlaceHolderHint,
                    self.xTextDocument)
                AgendaTemplate.trDate = i
            elif text == self.templateConsts.FILLIN_TIME:
                AgendaTemplate.teTime = PlaceholderTextElement(
                    i, self.resources.resPlaceHolderTime,
                    self.resources.resPlaceHolderHint,
                    self.xTextDocument)
                AgendaTemplate.trTime = i
            elif text == self.templateConsts.FILLIN_LOCATION:
                AgendaTemplate.teLocation = PlaceholderTextElement(
                    i, self.resources.resPlaceHolderLocation,
                    self.resources.resPlaceHolderHint,
                    self.xTextDocument)
                AgendaTemplate.trLocation = i
            else:
                auxList.append(i)
        self.allItems = auxList

    '''
    analyze the item sections in the template.
    delegates the analyze of each table to the ItemsTable class.
    '''

    def initializeItemsSections(self):
        sections = self.getSections(
        self.xTextDocument, self.templateConsts.SECTION_ITEMS)
        # for each section - there is a table...
        self.itemsTables = []
        for i in sections:
            try:
                self.itemsTables.append(
                    ItemsTable(self.getSection(i), self.getTable(i), self))
            except Exception:
                traceback.print_exc()
                raise AttributeError (
                    "Fatal Error while initialilzing \
                    Template: items table in section " + i)


    def getSections(self, document, s):
        allSections = document.TextSections.ElementNames
        return self.getNamesWhichStartWith(allSections, s)

    def getSection(self, name):
        return getattr(self.xTextDocument.TextSections, name)

    def getTable(self, name):
        return getattr(self.xTextDocument.TextTables, name)

    def redrawTitle(self, controlName):
        try:
            if controlName == "txtTitle":
                self.writeTitle(
                    AgendaTemplate.teTitle, AgendaTemplate.trTitle,
                    self.agenda.cp_Title)
            elif controlName == "txtDate":
                self.writeTitle(
                    AgendaTemplate.teDate, AgendaTemplate.trDate,
                    self.getDateString(self.agenda.cp_Date))
            elif controlName == "txtTime":
                self.writeTitle(
                    AgendaTemplate.teTime, AgendaTemplate.trTime,
                    self.getTimeString(self.agenda.cp_Time))
            elif controlName == "cbLocation":
                self.writeTitle(
                    AgendaTemplate.teLocation, AgendaTemplate.trLocation,
                    self.agenda.cp_Location)
            else:
                raise IllegalArgumentException ("No such title control...")
        except Exception:
            traceback.print_exc()

    @classmethod
    def writeTitle(self, te, tr, text):
        if text is None:
            te.placeHolderText = ""
        else:
            te.placeHolderText = text
        te.write(tr)

    @classmethod
    def getDateString(self, d):
        if d is None or d == "":
            return ""
        date = int(d)
        year = int(date / 10000)
        month = int((date % 10000) / 100)
        day = int(date % 100)
        dateObject = dateTimeObject(year, month, day)
        return AgendaTemplate.dateUtils.format(
            AgendaTemplate.dateFormat, dateObject)

    @classmethod
    def getTimeString(self, s):
        if s is None or s == "":
            return ""
        time = int(s)
        t = ((time / float(1000000)) / float(24)) \
            + ((time % 1000000) / float(1000000)) / float(35)
        return self.formatter.convertNumberToString(
            AgendaTemplate.timeFormat, t)

    def finish(self, topics):
        self.createMinutes(topics)
        self.deleteHiddenSections()
        self.textSectionHandler.removeAllTextSections()

    '''
    hidden sections exist when an item's section is hidden because the
    user specified not to display any items which it contains.
    When finishing the wizard removes this sections
    entirely from the document.
    '''

    def deleteHiddenSections(self):
        allSections = self.xTextDocument.TextSections.ElementNames
        try:
            for i in allSections:
                self.section = self.getSection(i)
                visible = bool(Helper.getUnoPropertyValue(
                    self.section, "IsVisible"))
                if not visible:
                    self.section.Anchor.String = ""

        except Exception:
            traceback.print_exc()

    '''
    create the minutes for the given topics or remove the minutes
    section from the document.
    If no topics are supplied, or the user specified not to create minuts,
    the minutes section will be removed,
    @param topicsData supplies PropertyValue arrays containing
    the values for the topics.
    '''

    def createMinutes(self, topicsData):
        # if the minutes section should be removed (the
        # user did not check "create minutes")
        if not self.agenda.cp_IncludeMinutes \
                or len(topicsData) <= 1:
            try:
                minutesAllSection = self.getSection(
                    self.templateConsts.SECTION_MINUTES_ALL)
                minutesAllSection.Anchor.String = ""
            except Exception:
                traceback.print_exc()

        # the user checked "create minutes"
        else:
            try:
                topicStartTime = int(self.agenda.cp_Time)
                #first I replace the minutes titles...
                self.items = self.searchFillInItems()
                itemIndex = 0
                for item in self.items:
                    itemText = item.String.lstrip().lower()
                    if itemText == \
                            self.templateConsts.FILLIN_MINUTES_TITLE:
                        self.fillMinutesItem(
                            item, self.agenda.cp_Title,
                            self.resources.resPlaceHolderTitle)
                    elif itemText == \
                            self.templateConsts.FILLIN_MINUTES_LOCATION:
                        self.fillMinutesItem(
                            item, self.agenda.cp_Location,
                            self.resources.resPlaceHolderLocation)
                    elif itemText == \
                            self.templateConsts.FILLIN_MINUTES_DATE:
                        self.fillMinutesItem(
                            item, getDateString(self.agenda.cp_Date),
                            self.resources.resPlaceHolderDate)
                    elif itemText == \
                            self.templateConsts.FILLIN_MINUTES_TIME:
                        self.fillMinutesItem(
                            item, getTimeString(self.agenda.cp_Time),
                            self.resources.resPlaceHolderTime)

                self.items.clear()
                '''
                now add minutes for each topic.
                The template contains *one* minutes section, so
                we first use the one available, and then add a one...
                topics data has *always* an empty topic at the end...
                '''

                for i in xrange(len(topicsData) - 1):
                    topic = topicsData[i]
                    items = self.searchFillInItems()
                    itemIndex = 0
                    for item in items:
                        itemText = item.String.lstrip().lower()
                        if itemText == \
                                self.templateConsts.FILLIN_MINUTE_NUM:
                            fillMinutesItem(item, topic[0].Value, "")
                        elif itemText == \
                                self.templateConsts.FILLIN_MINUTE_TOPIC:
                            fillMinutesItem(item, topic[1].Value, "")
                        elif itemText == \
                                self.templateConsts.FILLIN_MINUTE_RESPONSIBLE:
                            fillMinutesItem(item, topic[2].Value, "")
                        elif itemText == \
                                self.templateConsts.FILLIN_MINUTE_TIME:
                            topicTime = 0
                            try:
                                topicTime = topic[3].Value
                            except Exception:
                                pass

                            '''
                            if the topic has no time, we do not
                            display any time here.
                            '''
                            if topicTime == 0 or topicStartTime == 0:
                                time = topic[3].Value
                            else:
                                time = getTimeString(str(topicStartTime)) + " - "
                                topicStartTime += topicTime * 1000
                                time += getTimeString(str(topicStartTime))

                            fillMinutesItem(item, time, "")

                    self.textSectionHandler.removeTextSectionbyName(
                        self.templateConsts.SECTION_MINUTES)
                    # after the last section we do not insert a one.
                    if i < len(topicsData) - 2:
                        self.textSectionHandler.insertTextSection(
                            self.templateConsts.SECTION_MINUTES,
                            self.template, False)

            except Exception:
                traceback.print_exc()

    '''given a text range and a text, fills the given
    text range with the given text.
    If the given text is empty, uses a placeholder with the giveb
    placeholder text.
    @param range text range to fill
    @param text the text to fill to the text range object.
    @param placeholder the placeholder text to use, if the
    text argument is empty (null or "")
    '''

    def fillMinutesItem(self, Range, text, placeholder):
        paraStyle = Helper.getUnoPropertyValue(Range, "ParaStyleName")
        Range.setString(text)
        Helper.setUnoPropertyValue(Range, "ParaStyleName", paraStyle)
        if text == None or text == "":
            if placeholder != None and not placeholder == "":
                placeHolder = createPlaceHolder(
                    self.xTextDocument, placeholder,
                    self.resources.resPlaceHolderHint)
                try:
                    Range.Start.Text.insertTextContent(
                        Range.Start, placeHolder, True)
                except Exception:
                    traceback.print_exc()

    '''
    creates a placeholder field with the given text and given hint.
    '''

    @classmethod
    def createPlaceHolder(self, xmsf, ph, hint):
        try:
            placeHolder =  xmsf.createInstance(
                "com.sun.star.text.TextField.JumpEdit")
        except Exception:
            traceback.print_exc()
            return None

        Helper.setUnoPropertyValue(placeHolder, "PlaceHolder", ph)
        Helper.setUnoPropertyValue(placeHolder, "Hint", hint)
        Helper.setUnoPropertyValue(
           placeHolder, "PlaceHolderType", uno.Any("short",TEXT))
        return placeHolder

    def getNamesWhichStartWith(self, allNames, prefix):
        v = []
        for i in allNames:
            if i.startswith(prefix):
                v.append(i)
        return v

    '''
    Convenience method for inserting some cells into a table.
    '''

    @classmethod
    def insertTableRows(self, table, start, count):
        rows = table.Rows
        rows.insertByIndex(start, count)

    '''returns the row index for this cell name.
    @param cellName
    @return the row index for this cell name.
    '''

    @classmethod
    def getRowIndex(self, cellName):
        return int(cellName.RangeName[1:])

    '''
    returns the rows count of this table, assuming
    there is no vertical merged cells.
    '''

    @classmethod
    def getRowCount(self, table):
        cells = table.getCellNames()
        return int(cells[len(cells) - 1][1:])

class ItemsTable(object):
    '''
    the items in the table.
    '''
    items = []
    table = None

    def __init__(self, section, table, agenda):
        self.agenda = agenda
        ItemsTable.table = table
        self.section = section
        self.items = []
        '''
        go through all <*> items in the document
        and each one if it is in this table.
        If they are, register them to belong here, notice their order
        and remove them from the list of all <*> items, so the next
        search will be faster.
        '''
        i = 0
        while i < len(self.agenda.allItems):
            workwith = self.agenda.allItems[i]
            t = Helper.getUnoPropertyValue(workwith, "TextTable")
            if t == ItemsTable.table:
                iText = workwith.String.lower().lstrip()
                ai = self.agenda.itemsCache[iText]
                if ai is not None:
                    self.items.append(ai)
                    del self.agenda.allItems[i]
                    self.agenda.itemsMap[iText] = self
                    i -= 1
            i += 1

    '''
    link the section to the template. this will restore the original table
    with all the items.<br/>
    then break the link, to make the section editable.<br/>
    then, starting at cell one, write all items that should be visible.
    then clear the rest and remove obsolete rows.
    If no items are visible, hide the section.
    @param dummy we need a param to make this an Implementation
    of AgendaElement.
    @throws Exception
    '''

    def write(self, dummy):
        name = self.section.Name
        # link and unlink the section to the template.
        self.agenda.textSectionHandler.linkSectiontoTemplate(
            self.agenda.template, name, self.section)
        self.agenda.textSectionHandler.breakLinkOfTextSection(
            self.section)
        # we need to get a instance after linking.
        ItemsTable.table = self.agenda.getTable(name)
        self.section = self.agenda.getSection(name)
        cursor = ItemsTable.table.createCursorByCellName("A1")
        # should this section be visible?
        visible = False
        # write items
        # ===========
        cellName = ""
        '''
        now go through all items that belong to this
        table. Check each one agains the model. If it should
        be display, call it's write method.
        All items are of type AgendaItem which means they write
        two cells to the table: a title (text) and a placeholder.
        see AgendaItem class below.
        '''
        for i in self.items:
            if self.agenda.isShowItem(i.name):
                visible = True
                i.table = ItemsTable.table
                i.write(cursor)
                # I store the cell name which was last written...
                cellName = cursor.RangeName
                cursor.goRight(1, False)

        if visible:
            boolean = True
        else:
            boolean = False
        Helper.setUnoPropertyValue(self.section, "IsVisible", boolean)
        if not visible:
            return
            '''
            remove obsolete rows
            ====================
            if the cell that was last written is the current cell,
            it means this is the end of the table, so we end here.
            (because after getting the cellName above,
            I call the goRight method.
            If it did not go right, it means its the last cell.
            '''

        if cellName == cursor.RangeName:
            return
            '''
            if not, we continue and clear all cells until
            we are at the end of the row.
            '''

        while not cellName == cursor.RangeName and \
                not cursor.RangeName.startswith("A"):
            cell = ItemsTable.table.getCellByName(cursor.RangeName)
            cell.String = ""
            cellName = cursor.RangeName
            cursor.goRight(1, False)

        '''
        again: if we are at the end of the table, end here.
        '''
        if cellName == cursor.RangeName:
            return

        rowIndex = AgendaTemplate.getRowIndex(cursor)
        rowsCount = AgendaTemplate.getRowCount(ItemsTable.table)
        '''
        now before deleteing i move the cursor up so it
        does not disappear, because it will crash office.
        '''
        cursor.gotoStart(False)

'''
This class handles the preview of the topics table.
You can call it the controller of the topics table.
It differs from ItemsTable in that it has no data model -
the update is done programttically.<br/>
<br/>
The decision to make this class a class by its own
was done out of logic reasons and not design/functionality reasons,
since there is anyway only one instance of this class at runtime
it could have also be implemented in the AgendaTemplate class
but for clarity and separation I decided to make a sub class for it.

@author rp143992
'''

class Topics(object):
    '''Analyze the structure of the Topics table.
    The structure Must be as follows:<br>
    -One Header Row. <br>
    -arbitrary number of rows per topic <br>
    -arbitrary content in the topics row <br>
    -only soft formatting will be restored. <br>
    -the topic rows must repeat three times. <br>
    -in the topics rows, placeholders for number, topic, responsible,
    and duration must be placed.<br><br>
    A word about table format: to reconstruct the format of the table we hold
    to the following formats: first row (header), topic, and last row.
    We hold the format of the last row, because one might wish to give it
    a special format, other than the one on the bottom of each topic.
    The left and right borders of the whole table are, on the other side,
    part of the topics rows format, and need not be preserved seperateley.
    '''
    table = None
    lastRowFormat = []
    numCell = -1
    topicCell = -1
    responsibleCell = -1
    timeCell = -1
    rowsPerTopic = None
    topicCells = []

    def __init__(self, agenda):
        self.topicItems = {}
        self.firstRowFormat = []
        self.agenda = agenda
        '''
        this is a list which traces which topics were written to the document
        and which not. When a cell needs to be actualized, it is checked that the
        whole topic is already present in the document, using this vector.
        The vector contains nulls for topics which were not written, and
        empty strings for topics which were written (though any other
        object would also do - i check only if it is a null or not...);
        '''
        self.writtenTopics = []
        try:
            Topics.table = self.agenda.getTable(
                self.agenda.templateConsts.SECTION_TOPICS)
        except Exception:
            traceback.print_exc()
            raise AttributeError (
                "Fatal error while loading template: table " + \
                self.agenda.templateConsts.SECTION_TOPICS + " could not load.")

        '''
        first I store all <*> ranges
        which are in the topics table.
        I store each <*> range in this - the key
        is the cell it is in. Later when analyzing the topic,
        cell by cell, I check in this map to know
        if a cell contains a <*> or not.
        '''
        try:
            items = {}
            for i in self.agenda.allItems:
                t = Helper.getUnoPropertyValue(i, "TextTable")
                if t == Topics.table:
                    cell = Helper.getUnoPropertyValue(i, "Cell")
                    iText = cell.CellName
                    items[iText] = i

            '''
            in the topics table, there are always one
            title row and three topics defined.
            So no mutter how many rows a topic takes - we
            can restore its structure and format.
            '''
            rows = self.agenda.getRowCount(Topics.table)
            Topics.rowsPerTopic = int((rows - 1) / 3)

            firstCell = "A" + str(1 + Topics.rowsPerTopic + 1)
            afterLastCell = "A" + str(1 + (Topics.rowsPerTopic * 2) + 1)
            # go to the first row of the 2. topic

            cursor = Topics.table.createCursorByCellName(firstCell)
            # analyze the structure of the topic rows.
            while not cursor.RangeName == afterLastCell:
                cell = Topics.table.getCellByName(cursor.RangeName)
                # if the cell contains a relevant <...>
                # i add the text element to the hash,
                # so it's text can be updated later.
                try:
                    if items[cell.CellName] is not None:
                        self.topicItems[cell.String.lower().lstrip()] = cell
                except KeyError:
                    pass

                Topics.topicCells.append(cell)
                # goto next cell.
                cursor.goRight(1, False)
            '''
            now - in which cell is every fillin?
            '''

            Topics.numCell = Topics.topicCells.index(
                self.topicItems[
                    self.agenda.templateConsts.FILLIN_TOPIC_NUMBER])
            Topics.topicCell = Topics.topicCells.index(
                self.topicItems[
                    self.agenda.templateConsts.FILLIN_TOPIC_TOPIC])
            Topics.responsibleCell = Topics.topicCells.index(
                self.topicItems[
                    self.agenda.templateConsts.FILLIN_TOPIC_RESPONSIBLE])
            Topics.timeCell = Topics.topicCells.index(
                self.topicItems[
                    self.agenda.templateConsts.FILLIN_TOPIC_TIME])
        except Exception:
            traceback.print_exc()

    '''@param topic the topic number to write
    @param data the data of the topic.
    @return the number of rows that have been added
    to the table. 0 or a negative number: no rows added.
    '''

    def write2(self, topic, data):
        if topic >= len(self.writtenTopics):
            size = topic - len(self.writtenTopics)
            self.writtenTopics += [None] * size
        self.writtenTopics.insert(topic, "")
        # make sure threr are enough rows for me...
        rows = self.agenda.getRowCount(Topics.table)
        reqRows = 1 + (topic + 1) * Topics.rowsPerTopic
        firstRow = reqRows - Topics.rowsPerTopic + 1
        diff = reqRows - rows
        if diff > 0:
            self.agenda.insertTableRows(Topics.table, rows, diff)
            # set the item's text...

        self.setItemText(Topics.numCell, data[0].Value)
        self.setItemText(Topics.topicCell, data[1].Value)
        self.setItemText(Topics.responsibleCell, data[2].Value)
        self.setItemText(Topics.timeCell, data[3].Value)
        # now write !
        cursor = Topics.table.createCursorByCellName("A" + str(firstRow))
        for i in Topics.topicCells:
            i.write(Topics.table.getCellByName(cursor.RangeName))
            cursor.goRight(1, False)
        # now format !
        cursor.gotoCellByName("A" + str(firstRow), False)
        return diff

    '''check if the topic with the given index is written to the table.
    @param topic the topic number (0 base)
    @return true if the topic is already written to the table. False if not.
    (false would mean rows must be added to the table in order to
    be able to write this topic).
    '''

    def isWritten(self, topic):
        return (len(self.writtenTopics) > topic \
            and self.writtenTopics[topic] is not None)

    '''rewrites a single cell containing.
    This is used in order to refresh the topic/responsible/duration data
    in the preview document, in response to a change in the gui (by the user)
    Since the structure of the topics table is flexible,
    we don't reference a cell number. Rather, we use "what" argument to
    specify which cell should be redrawn.
    The Topics object, which analyzed the structure of the topics table appon
    initialization, refreshes the approperiate cell.
    @param topic index of the topic (0 based).
    @param what 0 for num, 1 for topic, 2 for responsible, 3 for duration
    @param data the row's data.
    @throws Exception if something goes wrong (thow nothing should)
    '''

    def writeCell(self, topic, what, data):
        # if the whole row should be written...
        if not self.isWritten(topic):
            self.write(topic, data)
            # write only the "what" cell.
        else:
            # calculate the table row.
            firstRow = 1 + (topic * Topics.rowsPerTopic) + 1
            # go to the first cell of this topic.
            cursor = Topics.table.createCursorByCellName("A" + str(firstRow))
            te = None
            cursorMoves = 0
            if what == 0:
                te = self.setItemText(Topics.numCell, data[0].Value)
                cursorMoves = Topics.numCell
            elif what == 1:
                te = self.setItemText(Topics.topicCell, data[1].Value)
                cursorMoves = Topics.topicCell
            elif what == 2:
                te = self.setItemText(Topics.responsibleCell, data[2].Value)
                cursorMoves = Topics.responsibleCell
            elif what == 3:
                te = self.setItemText(Topics.timeCell, data[3].Value)
                cursorMoves = Topics.timeCell

            # move the cursor to the needed cell...
            cursor.goRight(cursorMoves, False)
            xc = Topics.table.getCellByName(cursor.RangeName)
            # and write it !
            te.write(xc)

    '''writes the given topic.
    if the first topic was involved, reformat the
    first row.
    If any rows were added to the table, reformat
    the last row.
    @param topic the index of the topic to write.
    @param data the topic's data. (see TopicsControl
    for explanation about the topics data model)
    @throws Exception if something goes wrong (though nothing should).
    '''

    def write(self, topic, data):
        diff = self.write2(topic, data)
        '''if the first topic has been written,
        one needs to reformat the first row.
        '''
        if topic == 0:
            self.formatFirstRow()
        '''
        if any rows were added, one needs to format
        the whole table again.
        '''

        if diff > 0:
            self.formatLastRow()

    '''removes obsolete rows, reducing the
    topics table to the given number of topics.
    Note this method does only reducing - if
    the number of topics given is greater than the
    number of actuall topics it does *not* add
    rows !
    Note also that the first topic will never be removed.
    If the table contains no topics, the whole section will
    be removed uppon finishing.
    The reason for that is a "table-design" one: the first topic is
    maintained in order to be able to add rows with a design of this topic,
    and not of the header row.
    @param topics the number of topics the table should contain.
    @throws Exception
    '''

    def reduceDocumentTo(self, topics):
        # we never remove the first topic...
        if topics <= 0:
            topics = 1

        tableRows = Topics.table.Rows
        targetNumOfRows = topics * Topics.rowsPerTopic + 1
        '''if tableRows.Count > targetNumOfRows:
            tableRows.removeByIndex(
                targetNumOfRows, tableRows.Count - targetNumOfRows)'''

        self.formatLastRow()
        while len(self.writtenTopics) > topics:
            del self.writtenTopics[topics]

    '''reapply the format of the first (header) row.
    '''

    def formatFirstRow(self):
        cursor = Topics.table.createCursorByCellName("A1")
        self.formatTable(cursor, self.firstRowFormat, False)

    '''reaply the format of the last row.
    '''
    @classmethod
    def formatLastRow(self):
        cursor = Topics.table.createCursorByCellName("A1")
        cursor.gotoEnd(False)
        self.formatTable(cursor, Topics.lastRowFormat, True)

    '''returns a text element for the given cell,
    which will write the given text.
    @param cell the topics cell number.
    @param value the value to write.
    @return a TextElement object which will write the given value
    to the given cell.
    '''

    @classmethod
    def setItemText(self, cell, value):
        if cell >= 0:
            te = Topics.topicCells[cell]
            if te is not None:
                te.Text = str(value)
            return te

        return None

    '''formats a series of cells from the given one,
    using the given List of TableCellFormatter objects,
    in the given order.
    This method is used to format the first (header) and the last
    rows of the table.
    @param cursor a table cursor, pointing to the start cell to format
    @param formats a List containing TableCellFormatter objects.
    Each will format one cell in the direction specified.
    @param reverse if true the cursor will move left,
    formatting in reverse order (used for the last row).
    '''
    @classmethod
    def formatTable(self, cursor, formats, reverse):
        for i in formats:
            i.format(Topics.table.getCellByName(cursor.RangeName))
            if reverse:
                cursor.goLeft(1, False)
            else:
                cursor.goRight(1, False)

'''
A Text element which, if the text to write is empty (null or "")
inserts a placeholder instead.
@author rp143992
'''

class PlaceholderTextElement(TextElement):

    def __init__(self, textRange, placeHolderText_, hint_, xmsf_):
        super(PlaceholderTextElement,self).__init__(textRange, "")

        self.text = placeHolderText_
        self.hint = hint_
        self.xmsf = xmsf_

    def write(self, textRange):
        textRange.String = self.placeHolderText
        if self.placeHolderText is None or self.placeHolderText == "":
            try:
                xTextContent = AgendaTemplate.createPlaceHolder(
                    self.xmsf, self.text, self.hint)
                textRange.Text.insertTextContent(
                    textRange.Start, xTextContent, True)
            except Exception:
                traceback.print_exc()

'''
An Agenda element which writes no text, but inserts a placeholder, and formats
it using a ParaStyleName.
@author rp143992
'''

class PlaceholderElement(object):

    def __init__(self, placeHolderText_, hint_,  textDocument):
        self.placeHolderText = placeHolderText_
        self.hint = hint_
        self.textDocument =  textDocument

    def write(self, textRange):
        try:
            xTextContent = AgendaTemplate.createPlaceHolder(
                self.textDocument, self.placeHolderText, self.hint)
            textRange.Text.insertTextContent(
                textRange.Start, xTextContent, True)
        except Exception:
            traceback.print_exc()

'''
An implementation of AgendaElement which
gets as a parameter a table cursor, and writes
a text to the cell marked by this table cursor, and
a place holder to the next cell.
@author rp143992
'''

class AgendaItem(object):

    def __init__(self, name_, te, f):
        self.name = name_
        self.field = f
        self.textElement = te

    def write(self, tableCursor):
        cellname = tableCursor.RangeName
        cell = ItemsTable.table.getCellByName(cellname)
        cell.String = self.textElement
        tableCursor.goRight(1, False)
        #second field is actually always null...
        # this is a preparation for adding placeholders.
        if self.field is not None:
            self.field.write(ItemsTable.table.getCellByName(
                tableCursor.RangeName))
